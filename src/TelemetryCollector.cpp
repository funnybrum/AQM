#include "AQMonitor.h"

#define PUSH_INTERVAL 120
#define COLLECT_INTERVAL 2
#define DATABASE "test"

void TelemetryCollector::begin() {
    lastDataCollect = millis() - COLLECT_INTERVAL * 1000;
    lastDataPush = millis();

    http = new HTTPClient();
    const char * headerKeys[] = {"date"};
    http->collectHeaders(headerKeys, 1);

    int httpCode = -1;
    int retryCount = 0;
    while (httpCode != 204 && retryCount < 3) {
        http->begin("http://192.168.0.200:8086/ping");
        httpCode = http->GET();
        if (httpCode != 204) {
            http->end();
        }
        retryCount += 1;
        delay(100);
    }

    if (httpCode == 204) {
        syncTime(http->header("date").c_str());
        http->end();
    } else {
        logger.log("Failed to get timestmap from the InfluxDB server!");
    }

}

void TelemetryCollector::loop() {
    if (millis() - lastDataCollect > COLLECT_INTERVAL * 1000) {
        collect();
        lastDataCollect += COLLECT_INTERVAL * 1000;
        delay(2000);
    }

    if (millis() - lastDataPush > PUSH_INTERVAL * 1000) {
        if (push()) {
            lastDataPush += PUSH_INTERVAL * 1000;
        }
    }

    // if (millis() - remoteTimestampMillis > 5000) {
    //     http->begin("http://192.168.0.200:8086/ping");
    //     int httpCode = http->GET();
    //     if (httpCode < 300) {
    //         syncTime(http->header("date").c_str());
    //     } else {
    //         Serial.printf("status code: %d\n", httpCode);
    //     }
    //     http->end();
    //     Serial.printf("%ld: timestamp=%ld\n", count, getTimestamp());
    //     count++;
    // }
}

void TelemetryCollector::syncTime(const char* dateTime) {
    Serial.println(dateTime);
    // Get the millis when the remote date was received.
    remoteTimestampMillis = millis();

    // Calculate the timestamp from a date/time string as "Sat, 08 Dec 2018 07:38:17 GMT". Based on
    // the "Seconds Since the Epoch" formula as defined by POSIX:2008 section 4.15. The following
    // are the required parameters:
    int16_t tm_sec = atoi(dateTime+23);         // seconds
    int16_t tm_min = atoi(dateTime+20);         // minutes
    int16_t tm_hour = atoi(dateTime+17);        // hours
    int16_t tm_year = atoi(dateTime+12) - 1900; // calendar year minus 1900
    int16_t tm_yday;                            // passed days since January 1 of the current year

    // tm_yday is calculated based on the month, the day and on the year (leap/non-leap).

    // Day in current month
    int16_t day = atoi(dateTime+5);

    // Convert month to number. 1 for January, 12 for December.
    const char* months[12] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    int16_t month = 1;
    while (strncmp(dateTime + 8, months[month-1], 3) != 0 && month < 13) {
        month++;
    }

    // Determine if the year is leap or not.
	bool isLeapYear = false;
	if ((tm_year + 1900) % 4 == 0 )
		isLeapYear = true;
	if ((tm_year + 1900) % 100 == 0 )
		isLeapYear = false;
	if ((tm_year + 1900) % 400 == 100 )
        isLeapYear = true;

    // Calculate the number of passed days before the current month.
    int16_t days_before_month[2][13] = {
        /* Normal years.  */
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
        /* Leap years.  */
        {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
    };

    // Calculate the full days that have passed since the year start.
    tm_yday = days_before_month[isLeapYear][month-1] + day-1;

    remoteTimestamp =
        tm_sec + tm_min*60 +
        tm_hour*3600 +
        tm_yday*86400 +
        (tm_year-70)*31536000 +
        ((tm_year-69)/4)*86400 -
        ((tm_year-1)/100)*86400 +
        ((tm_year+299)/400)*86400;
}

unsigned long TelemetryCollector::getTimestamp() {
    // The below would be correct even if the millis() have rolled over. The only requirement for
    // this to work is to have the last timestamp retrieved before less than the millis rollover
    // period that is ~50 days.
    unsigned long secondsSinceTimestampRetrieval = (millis() - remoteTimestampMillis) / 1000;

    return remoteTimestamp + secondsSinceTimestampRetrieval;
}

void TelemetryCollector::append(const char* metric, float value) {
    data +=
        String(metric) +
        ",hostname=" +
        settings.get()->hostname +
        " value=" +
        value +
        " " +
        getTimestamp() +
        "000000000\n";
}

void TelemetryCollector::collect() {
    append("iaq", aqSensors.getIAQ());
    append("iaq_static", aqSensors.getStaticIAQ());
    append("iaq_calculated", aqSensors.getCalculatedIAQ());
    append("temperature", aqSensors.getTemp());
    append("humidity", aqSensors.getHumidity());
    append("resistance", aqSensors.getGasResistance());
    append("pressure", aqSensors.getPressure());
    append("accuracy", aqSensors.getAccuracy());
    Serial.printf("Free memory: %ld\n", ESP.getFreeHeap());
}

bool TelemetryCollector::push() {
    http->begin(String("http://192.168.0.200:8086/write?db=") + DATABASE);
    int statusCode = http->POST((uint8_t *)data.c_str(), data.length()-1);  // Size is -1 to remove the last '\n'.
    if (statusCode == 204) {
        data.remove(0);
        syncTime(http->header("date").c_str());
        return true;
    } else {
        Serial.printf("Got %d status code...", statusCode);
    }

    return false;
}

TelemetryCollector telemetryCollector = TelemetryCollector();
