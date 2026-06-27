# Documentation
 | ArduinoJson 7

Source: https://arduinojson.org/v7/

## First contact





### [Installation](https://arduinojson.org/v7/how-to/install-arduinojson/)
 How to install ArduinoJson on your computer.



### [Deserialization tutorial](https://arduinojson.org/v7/tutorial/deserialization/)
 Step-by-step guide on how to parse a JSON document using the library.



### [Serialization tutorial](https://arduinojson.org/v7/tutorial/serialization/)
 Step-by-step guide on how to generate a JSON document using the library.



### [Examples](https://arduinojson.org/v7/example/)
 Complete programs showing how to use ArduinoJson in various situations.





## Everyday use





### [API Reference](https://arduinojson.org/v7/api/)
 Detailed description of each function of the library.



### [Compile time configuration](https://arduinojson.org/v7/config/)
 Settings to configure the library



### [How To’s](https://arduinojson.org/v7/how-to/)
 Recipes and techniques to use ArduinoJson in your project.



### [FAQ](https://arduinojson.org/v7/faq/)
 The answers to the most frequent questions.



### [Release notes](https://arduinojson.org/v7/revisions/)
 See what changed between each version.



### [Known issues](https://arduinojson.org/v7/known-issues/)
 All the known issues and limitations of the current version.


---


# Deserialization tutorial
 | ArduinoJson 7

Source: https://arduinojson.org/v7/tutorial/deserialization/

In this quick tutorial, we’ll see how to deserialize a JSON document with ArduinoJson 7. As a reminder, deserialization turns a JSON input into objects from which we can extract the values. This tutorial requires a basic understanding of Arduino programming and a vague knowledge of JSON.



It doesn’t matter if you’re using an Arduino UNO, an ESP8266, or an ESP32; the code shown here works on any device.



## The JSON input



We’ll write a program that extracts and prints all the values from the following JSON document:



```cpp
{
  "sensor": "gps",
  "time": 1351824120,
  "data": [
    48.756080,
    2.302038
  ]
}
```



This JSON document contains three root members: a string named `sensor`, a number named `time`, and an array named `data`.



We’ll store the input in the following variable:



```cpp
const char* input = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
```



In this case, the JSON input is hard-coded, but in an actual project, it could come from an HTTP response or a configuration file.



## Deserializing the document



I’m assuming you already installed ArduinoJson 7 on your computer. If not, please refer to the [installation guide](https://arduinojson.org/v7/how-to/install-arduinojson/).



First, we must include the library in the program. Add the following line at the top of your program:



```cpp
#include <ArduinoJson.h>
```



Then, we must declare a variable to store the deserialized document. We do that with [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/):



```cpp
JsonDocument doc;
```



Then, we must call [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/), passing the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) as the first argument and the JSON input as the second.



```cpp
deserializeJson(doc, input);
```



## Extracting the values



We can now extract the values from the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/):



```cpp
const char* sensor = doc["sensor"];
```



As you can see, we use square brackets to extract a member from a JSON object.



ArduinoJson implicitly converts the value to the expected type.
 We can do the same with the timestamp:



```cpp
long time = doc["time"];
```



If you don’t like implicit conversions, you can explicitly set the type with [`as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/).



Finally, you can chain brackets to extract nested values. For example, we can extract the coordinates like so:



```cpp
double latitude = doc["data"][0];
double longitude = doc["data"][1];
```



Here, we are extracting only two values from the `data` array, but if we were to extract many values, it would be more efficient to save a reference to the array.



```cpp
JsonArray data = doc["data"];
double latitude = data[0];
double longitude = data[1];
```



As you can see, we use [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) to hold a reference to the array in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). This is more efficient than the previous version because the program only searches the `"data"` key once. Also, [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) allows you to perform loops, like so:



```cpp
for (JsonVariant item : data) {

}
```



As you can see, the item in the for loop is a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/). Like [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), it is a reference to a value in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), except [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is limited to arrays, whereas [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) can refer to any value.



It is crucial to understand that neither [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) nor [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) holds any data. Instead, they rely on the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to have it. Therefore, you must never use a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) or a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) after the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) has been destructed



Note that there is a third reference type, [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), similar to [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), but for objects. I invite you to check the documentation to learn more.



Now that we have extracted the values, we can print them to the serial port.



```cpp
Serial.println(sensor);
Serial.println(time);
Serial.println(latitude, 6);
Serial.println(longitude, 6);
```



## Error handling



If you run this program, you’ll see that is prints the values as expected, but we forgot an essential part: the error handling.



Two kinds of problem can occur in this program: deserialization errors and extraction errors.



### Deserialization errors



The first kind of problem is an issue with the deserialization process. For example, the JSON input could be invalid, or we could run out of memory. We can easily detect these problems by checking the return value of [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/).



```cpp
DeserializationError error = deserializeJson(doc, input);
```



As you can see, [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) returns an error status of type [`DeserializationError`](https://arduinojson.org/v7/api/misc/deserializationerror/).



We can test if the call failed with `if(error)`:



```cpp
if (error) {
  Serial.print("deserializeJson() returned ");
  Serial.println(error.c_str());
  return;
}
```



As you can see, we can call `c_str()` to get a string representation of the error.



### Extraction errors



The second kind of problem is when deserialization succeeds, but the document differs from what the program expects. For example, some fields could be missing or have a different type.



In both cases, ArduinoJson returns a default value that depends on the extracted type. For example, if the member `sensor` is missing, or if it’s not a string, ArduinoJson will return a null pointer:



```cpp
const char* sensor = doc["sensor"]; // default = nullptr
```



In other words, the variable `sensor` would be null, so you may need to prepare for this possibility. Similarly, if the member `time` is missing, ArduinoJson will return 0:



```cpp
long time = doc["time"]; // default = 0
```



You can change the default value using [the pipe operator](https://arduinojson.org/v7/api/jsonvariant/or/). For example, you could define a default for `sensor` like so:



```cpp
const char* sensor = doc["sensor"] | "unknown";
```



Now, if `sensor` is missing, or if it’s not a string, the variable will be assigned to `"unknown"`.



Note that this particular use of [the pipe operator](https://arduinojson.org/v7/api/jsonvariant/or/) is specific to ArduinoJson and only works with [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) and [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/).



## The full program



Here is the full program:



```cpp
#include <ArduinoJson.h>

void setup() {
  // Initialize the serial port
  Serial.begin(9600);
  while (!Serial) continue;

  // {
  //   "sensor": "gps",
  //   "time": 1351824120,
  //   "data": [
  //     48.756080,
  //     2.302038
  //   ]
  // }
  const char* json = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Extract the values
  const char* sensor = doc["sensor"];
  long time = doc["time"];
  double latitude = doc["data"][0];
  double longitude = doc["data"][1];

  // Print the values
  Serial.println(sensor);
  Serial.println(time);
  Serial.println(latitude, 6);
  Serial.println(longitude, 6);
}

void loop() {
  // not used in this tutorial
}
```



Astute readers will recognize [JsonParserExample.ino](https://arduinojson.org/v7/example/parser/).



## ArduinoJson Assistant



As you saw in this tutorial, a significant part of the code is dedicated to extracting the values from the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). Writing this code can be cumbersome and error-prone, which is why ArduinoJson offers a tool to generate this code for you: the [ArduinoJson Assistant](https://arduinojson.org/v7/assistant/).



The [ArduinoJson Assistant](https://arduinojson.org/v7/assistant/) is an online tool available on arduinojson.org. You can access it by clicking “Assistant” at the top of the page.



The Assistant is composed of three steps.



### Step 1



In the first step, you must select:


* the device you’re using (e.g., Arduino UNO, ESP8266, ESP32),
* the mode you want to use: “Deserialize”,
* the input type (e.g., `const char*`).



### Step 2



In the second step, you must enter the JSON document you want to deserialize.



At the bottom, you can see some gages to check that this JSON document fits the hardware and software limitations.



### Step 3



The last step contains a sample program that extracts all the values from the JSON document entered in step 2.



You can copy this program and paste it into your Arduino IDE.
 Pretty cool, huh?



If you enjoyed learning from it and want to expand your skills further, consider checking out the full-length guide in my book [Mastering ArduinoJson](https://arduinojson.org/book/). Not only does it offer an extensive exploration of various aspects of JSON processing with Arduino, but purchasing the book directly supports the development of ArduinoJson and the other Arduino libraries I created.


---


# Serialization tutorial
 | ArduinoJson 7

Source: https://arduinojson.org/v7/tutorial/serialization/

In this very short tutorial, I’ll show you how to produce JSON documents easily with the ArduinoJson library.



Before starting, make sure that the library is installed and add the following statement at the top of your program:



```cpp
#include <ArduinoJson.h>
```



## The JSON output



In this tutorial, we’ll write a program that generates the following output to the serial port:



```cpp
{
  "sensor": "gps",
  "time": 1351824120,
  "data": [
    48.75608,
    2.302038
  ]
}
```



As you can see, it’s a JSON object containing three members: a string named “sensor”, an integer named “time”, and an array named “data”.



## Storing simple values



Before producing a JSON output, we must store all the values in a variable. With ArduinoJson, this is done with the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) class:



```cpp
JsonDocument doc;
```



Then, we add the values using brackets and the name of each object member. The first two members are straightforward:



```cpp
doc["sensor"] = "gps";
doc["time"] = 1351824120;
```



Things get slightly more complicated for the array.



## Storing arrays



ArduinoJson offers multiple syntaxes for adding values to an array.



First, you could use the brackets to assign an element at the specified index:



```cpp
doc["data"][0] = 48.75608;
doc["data"][1] = 2.302038;
```



Or, you could call [`add()`](https://arduinojson.org/v7/api/jsonvariant/add/) to append a new element at the end of the array:



```cpp
doc["data"].add(48.75608);
doc["data"].add(2.302038);
```



In both cases, however, the program performs the name lookup twice, which is inefficient. Indeed, you can see that the string “data” appears twice, forcing [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to look for the member twice.



We can avoid repeated lookups by storing a reference to the array:



```cpp
JsonArray data = doc["data"].to<JsonArray>();
data.add(48.75608);
data.add(2.302038);
```



Notice the call to [`to<JsonArray>()`](https://arduinojson.org/v7/api/jsonvariant/to/), which tells the library to convert the object member *to* an array. With the two previous syntaxes, the conversion was implicit, but here, we need to be explicit.



[`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) doesn’t hold any memory; instead, it refers to the data inside the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). Therefore, you can only use a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) if the corresponding [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is alive.



Similarly, ArduinoJson provides [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), which refers to a JSON object in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## Producing the JSON string



Now that the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) contains the data, we can produce a JSON string from it. In ArduinoJson, this is done with the [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) function:



```cpp
char output[256];
serializeJson(doc, output);
```



As you can see, you must pass the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) as the first argument and the destination variable as the second argument. The destination can be a char array, a string, or a stream.



For example, if you want to send the JSON output directly to the serial port, you can pass [`Serial`](https://www.arduino.cc/reference/en/language/functions/communication/serial/) as the second argument.



```cpp
serializeJson(doc, Serial);
```



[`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) produces minified JSON output; if you need an indented JSON output, you must call [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/) instead.



## The ArduinoJson Assistant



As you can see, the most challenging part of this tutorial was filling out the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) correctly. To help you in this task, we provide the [ArduinoJson Assistant](https://arduinojson.org/v7/assistant/), an online tool that scaffolds the code for you.



The [ArduinoJson Assistant](https://arduinojson.org/v7/assistant/) is designed as a three-step wizard.



In the first step, you select the microcontroller, choose either serialization or deserialization, and select the output type. Knowing the microcontroller allows the Assistant to estimate the memory consumption and tell you immediately if the JSON document can fit in the RAM.



In the second step, you must enter the JSON document you want to generate. Below the document, you’ll see a few gages that tell you how far away you are from the software and hardware limits.



In the third step, the Assistant provides a sample program to generate the document you specified in step 2. You can use this code as a starting point for your project.



## Conclusion



That’s all for this express tutorial on serialization. It should get you started with the most simple tasks, and you should now be able to understand the rest of the documentation.



If you need a more thorough introduction to serialization with ArduinoJson, I invite you to read the fourth chapter of my book, [Mastering ArduinoJson](https://arduinojson.org/book/). Not only does it explain how to produce a JSON string, but it also shows how to send the JSON document as part of an HTTP request with a real-life example using [Adafruit IO](https://io.adafruit.com/).



Of course, you’ll find many other exciting things in the book, including another tutorial for deserialization, advanced library features, and several case studies. Purchasing this book is also an excellent way to support the development of the library 💖.



## See also


* [Deserialization tutorial](https://arduinojson.org/v7/tutorial/deserialization/)
* [Mastering ArduinoJson](https://arduinojson.org/book/)


---


# API Reference
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/

This is the detailed documentation of every class and function of the ArduinoJson library. Some parts have been simplified to be easier to understand, so if you look at the source code, you might see some differences.



## Main classes





### [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/)
 A reference to a JSON array in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)







### [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/)
 A reference to a JSON object in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)







### [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/)
 A reference to a JSON value (array, object, integer…) in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)







### [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)
 Owns the memory for [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) and [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/)





## JSON serialization / deserialization





### [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)
 Deserialize a JSON document into a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).







### [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/)
 Serializes a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) into a minified JSON document







### [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/)
 Serializes a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) into a prettyfied JSON document







### [`measureJson()`](https://arduinojson.org/v7/api/json/measurejson/)
 Computes the size of the document that [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) produces







### [`measureJsonPretty()`](https://arduinojson.org/v7/api/json/measurejsonpretty/)
 Computes the size of the document that [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/) produces





## MessagePack serialization / deserialization





### [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/)
 Deserialize a MessagePack document into a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)







### [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/)
 Serializes a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) into a MessagePack document







### [`measureMsgPack()`](https://arduinojson.org/v7/api/msgpack/measuremsgpack/)
 Computes the size of the document that [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/) produces







### [`MsgPackBinary`](https://arduinojson.org/v7/api/msgpack/msgpackbinary/) 🆕
 Allows you to read or write MessagePack binary data







### [`MsgPackExtension`](https://arduinojson.org/v7/api/msgpack/msgpackextension/) 🆕
 Allows you to read or write MessagePack extension data





## Misc





### [`ARDUINOJSON_VERSION`](https://arduinojson.org/v7/api/misc/version/)
 Tells which version of ArduinoJson is installed







### [`copyArray()`](https://arduinojson.org/v7/api/misc/copyarray/)
 Copies values between a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) and a regular array.







### [`JsonString`](https://arduinojson.org/v7/api/jsonstring/)
 The string type returned by some functions.







### [`serialized()`](https://arduinojson.org/v7/api/misc/serialized/)
 Allows inserting preformatted pieces of JSON or MsgPack in a document


---


# How To’s
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/

## Upgrading


* [How to upgrade from ArduinoJson 6?](https://arduinojson.org/v7/how-to/upgrade-from-v6/)
* [How to upgrade from ArduinoJson 5?](https://arduinojson.org/v7/how-to/upgrade-from-v5/)



## Document


* [How to merge JSON objects?](https://arduinojson.org/v7/how-to/merge-json-objects/)
* [How to list the keys in a `JsonDocument`?](https://arduinojson.org/v7/how-to/list-keys-in-a-jsondocument/)
* [How to recursively search for a key?](https://arduinojson.org/v7/how-to/search-nested-key/)



## Converters


* [How to add support for `char`?](https://arduinojson.org/v7/how-to/add-support-for-char/)
* [How to create converters for STL containers?](https://arduinojson.org/v7/how-to/create-converters-for-stl-containers/)
* [How to create converters for `IPAddress`?](https://arduinojson.org/v7/how-to/create-converters-for-ipaddress/)



## Communication


* [How to do serial communication between two boards?](https://arduinojson.org/v7/how-to/do-serial-communication-between-two-boards/)
* [How to use ArduinoJson with ArduinoMqttClient?](https://arduinojson.org/v7/how-to/use-arduinojson-with-arduinomqttclient/)
* [How to use ArduinoJson with PubSubClient?](https://arduinojson.org/v7/how-to/use-arduinojson-with-pubsubclient/)



## Storage


* [How to store a JSON document in EEPROM?](https://arduinojson.org/v7/how-to/store-a-json-document-in-eeprom/)
* [How to append a JSON object to a file?](https://arduinojson.org/v7/how-to/append-to-a-file/)



## ArduinoJson on ESP8266 and ESP32


* [How to use ArduinoJson with `HTTPClient`?](https://arduinojson.org/v7/how-to/use-arduinojson-with-httpclient/)
* [How to use external RAM on ESP32?](https://arduinojson.org/v7/how-to/use-external-ram-on-esp32/)
* [How to use `string_view` on ESP32?](https://arduinojson.org/v7/how-to/use-string-view-on-esp32/)



## Deserialization


* [How to change the timeout when reading from a `Stream`?](https://arduinojson.org/v7/how-to/change-the-timeout/)
* [How to deserialize a very large document?](https://arduinojson.org/v7/how-to/deserialize-a-very-large-document/)
* [How to debug data coming from a stream?](https://arduinojson.org/v7/how-to/debug-data-coming-from-a-stream/)
* [How to validate a JSON document?](https://arduinojson.org/v7/how-to/validate-json/)



## Serialization


* [How to configure the serialization of floats?](https://arduinojson.org/v7/how-to/configure-the-serialization-of-floats/)
* [How to compute the hash of a JSON document?](https://arduinojson.org/v7/how-to/compute-hash/)



## Optimizations


* [How to improve (de)serialization speed?](https://arduinojson.org/v7/how-to/improve-speed/)
* [How to reduce memory usage?](https://arduinojson.org/v7/how-to/reduce-memory-usage/)



## Misc


* [How to use ArduinoJson with CMake?](https://arduinojson.org/v7/how-to/use-arduinojson-with-cmake/)
* [How to download the latest edition of Mastering ArduinoJson?](https://arduinojson.org/v7/how-to/upgrade-ebook/)
* [How to get the library version?](https://arduinojson.org/v7/how-to/get-library-version/)



## Cannot find what you’re looking for?

  Type in the box bellow to search in GitHub issues      Search


---


# FAQ
 | ArduinoJson 7

Source: https://arduinojson.org/v7/faq/

## Frequently Asked Questions


* [Can I automatically serialize an object?
How to convert a `struct` to a JSON object?](https://arduinojson.org/v7/faq/automatically-serialize-an-object/)
* [Why should I create a separate config object?
Why can’t I use `JsonDocument` directly?](https://arduinojson.org/v7/faq/why-should-i-create-a-separate-config-object/)
* [Why does the document take up so much RAM?](https://arduinojson.org/v7/faq/why-does-the-document-take-up-so-much-ram/)



## See also


* [How To’s](https://arduinojson.org/v7/how-to/)



## Cannot find what you’re looking for?

  Type in the box bellow to search in GitHub issues      Search


---


# How to install ArduinoJson 7


Source: https://arduinojson.org/v7/how-to/install-arduinojson/

Here are the common methods to install ArduinoJson.



## Method 1: Arduino IDE



Use this method if you use the Arduino IDE.


1. Click on the library tab in the Arduino IDE
2. Search for “ArduinoJson”
3. **Select the version: 7.4.3**
4. Click install.



## Method 2: PlatformIO



Use this method if you use PlatfomIO.


1. Click on the PlatformIO tab
2. Click on “Libraries”
3. Search for “ArduinoJson”
4. **Select the version: 7.4.3**
5. Click “Add to Project”



## Method 3: CMake



Use this method if you use CMake.



See [How to use ArduinoJson with CMake?](https://arduinojson.org/v7/how-to/use-arduinojson-with-cmake/)



## Method 4: Single header



Use this method if you use neither Arduino, PlatformIO, nor CMake.


1. Download [`ArduinoJson-v7.4.3.h`](https://github.com/bblanchon/ArduinoJson/releases/download/v7.4.3/ArduinoJson-v7.4.3.h)
2. Save the file alongside with your `.cpp` files



## Method 5: Build artifacts



Use this method to use the latest development version.


1. Download the [latest build artifacts](https://nightly.link/bblanchon/ArduinoJson/workflows/ci/7.x/Single%20headers.zip)
2. Extract `ArduinoJson.h` to your project folder.



## Method 6: Git clone



Use this method if you want to make changes to the library.


1. Run the command `git clone https://github.com/bblanchon/ArduinoJson.git`
2. Configure your compiler to add the folder `ArduinoJson/src/` to the list of include folders (`-I /path/to/ArduinoJson/src/` on GCC command line)


---


# JsonDocument
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/

## Description



`JsonDocument` stores a JSON document in memory. It owns the memory referenced by [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), and [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/).



## `JsonDocument` vs `JsonVariant`



`JsonDocument` shares many features with [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/); however, there is one big difference: `JsonDocument` has value semantics, whereas [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) has reference semantics.



On the one hand, because `JsonDocument` owns the data, if you copy a `JsonDocument`, you get a complete clone.



```cpp
// make a clone of the JsonDocument
JsonDocument doc2 = doc1;
```



On the other hand, because [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is a reference, if you copy a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), you only clone the reference:



```cpp
// make a new reference to the same variant
JsonVariant var2 = var1;
```



## Using a `JsonDocument`



When you create a `JsonDocument`, it is initially empty. At this stage, it’s neither an object, nor an array, and [`JsonDocument::isNull()`](https://arduinojson.org/v7/api/jsondocument/isnull/) returns `true`.



When you insert the first value in the `JsonDocument`, it automatically changes its type to match the call. If you use the `JsonDocument` like an array, it becomes an array; if you use the `JsonDocument` as an object, it becomes an object.



Here is a `JsonDocument` that implicitly becomes an object:



```cpp
JsonDocument doc;
doc["answer"] = 42;
// the doc contains {"answer":42}
```



Here is a `JsonDocument` that implicitly becomes an array:



```cpp
JsonDocument doc;
doc.add(42);
// the doc contains [42]
```



Sometimes, however, you’ll need to explicitly convert the `JsonDocument` without adding a value; for example, because you want to create an empty object. In this case you can call [`JsonDocument::to<T>()`](https://arduinojson.org/v7/api/jsondocument/to/):



```cpp
JsonDocument doc;
JsonObject obj = doc.to<JsonObject>();
// the doc contains {}
```



[`JsonDocument::to<T>()`](https://arduinojson.org/v7/api/jsondocument/to/) clears the document and converts it to the specified type. Don’t confuse this function with [`JsonDocument::as<T>()`](https://arduinojson.org/v7/api/jsondocument/as/) that returns a reference only if the requested type matches the one in the document.



## Allocator



By default, `JsonDocument` allocates memory through the standard `malloc()` and `free()` functions. You can customize this behavior by providing a custom allocator to the constructor.



You create an allocator class by inheriting from `ArduinoJson::Allocator` and implementing the pure virtual functions:



```cpp
namespace ArduinoJson {
class Allocator {
 public:
  virtual void* allocate(size_t size) = 0;
  virtual void deallocate(void* pointer) = 0;
  virtual void* reallocate(void* pointer, size_t new_size) = 0;
};
}
```



For example, here is an allocator that uses [ESP32’s capabilities-based heap memory allocator](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/mem_alloc.html):



```cpp
struct SpiRamAllocator : ArduinoJson::Allocator {
  void* allocate(size_t size) override {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) override {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) override {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};
```



Then you must pass an instance of this class to the constructor of `JsonDocument`:



```cpp
SpiRamAllocator allocator;
JsonDocument doc(&allocator);
```



## Member functions


* [`as<T>()`](https://arduinojson.org/v7/api/jsondocument/as/) casts the root to the specified type (e.g. [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) or [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/))
* [`add()`](https://arduinojson.org/v7/api/jsondocument/add/) adds elements to the root array
* [`clear()`](https://arduinojson.org/v7/api/jsondocument/clear/) empties the document and resets the memory pool
* [`containsKey()`](https://arduinojson.org/v7/api/jsondocument/containskey/) tests if the root object contains the specified key (deprecated)
* [`operator[]`](https://arduinojson.org/v7/api/jsondocument/subscript/) gets or sets values in the document
* [`overflowed()`](https://arduinojson.org/v7/api/jsondocument/overflowed/) tells if the memory pool was large enough
* [`is<T>()`](https://arduinojson.org/v7/api/jsondocument/is/) tests the type of the root
* [`isNull()`](https://arduinojson.org/v7/api/jsondocument/isnull/) tells if the document is null or empty
* [`nesting()`](https://arduinojson.org/v7/api/jsondocument/nesting/) returns the number of nesting layers in the document
* [`remove()`](https://arduinojson.org/v7/api/jsondocument/remove/) removes an element (or member) at the specified index (or key)
* [`set()`](https://arduinojson.org/v7/api/jsondocument/set/) replaces the root with the specified value
* [`shrinkToFit()`](https://arduinojson.org/v7/api/jsondocument/shrinktofit/) release overallocated memory.
* [`size()`](https://arduinojson.org/v7/api/jsondocument/size/) returns the number of elements (or members) that the root array (or object) contains
* [`to<T>()`](https://arduinojson.org/v7/api/jsondocument/to/) clears the document and converts it to the specified type (e.g. [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) or [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/))



## Example



```cpp
JsonDocument doc;

doc["hello"] = "world";

serializeJson(doc, Serial); // {"hello":"world"}
```



## See also


* [Serialization tutorial](https://arduinojson.org/v7/tutorial/serialization/)
* [Deserialization tutorial](https://arduinojson.org/v7/tutorial/deserialization/)


---


# deserializeJson()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/json/deserializejson/

## Description



The function `deserializeJson()` parses a JSON input and puts the result in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



Before reading the input, this function resets the document, so you don’t need to call [`JsonDocument::clear()`](https://arduinojson.org/v7/api/jsondocument/clear/).



## Signatures



```cpp
DeserializationError deserializeJson(JsonDocument& doc, const char* input);
DeserializationError deserializeJson(JsonDocument& doc, const char* input, size_t inputSize);
DeserializationError deserializeJson(JsonDocument& doc, const __FlashStringHelper* input);
DeserializationError deserializeJson(JsonDocument& doc, const __FlashStringHelper* input, size_t inputSize);
DeserializationError deserializeJson(JsonDocument& doc, const String& input);
DeserializationError deserializeJson(JsonDocument& doc, const std::string& input);
DeserializationError deserializeJson(JsonDocument& doc, Stream& input);
DeserializationError deserializeJson(JsonDocument& doc, std::istream& input);
DeserializationError deserializeJson(JsonDocument& doc, JsonVariantConst input);
DeserializationError deserializeJson(JsonDocument& doc, std::string_view input);

template<typename Reader> // custom reader class (see below)
DeserializationError deserializeJson(JsonDocument& doc, Reader& input);

// all overloads also accept optional parameters (see below)
```



## Arguments



`doc`: the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) that will store the memory representation of the JSON document.



`input`: the JSON document to parse:


* `const char*` is a string in RAM, it should be zero-terminated
* `const __FlashStringHelper*` is a Flash string, usually created with `F()`
* [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) is Arduino’s I/O stream interface, implemented by:
  * `HardwareSerial` (the class of [`Serial`](https://www.arduino.cc/reference/en/language/functions/communication/serial/))
  * [`SoftwareSerial`](https://www.arduino.cc/en/Reference/SoftwareSerial)
  * [`EthernetClient`](https://www.arduino.cc/en/Reference/EthernetClient)
  * [`WifiClient`](https://www.arduino.cc/en/Reference/WiFiClient)
  * `WiFiClientSecure`
  * `BluetoothClient`
  * [`EthernetUDP`](https://www.arduino.cc/en/Reference/EthernetUDPBegin)
  * [`GSMClient`](https://www.arduino.cc/en/Reference/GSM)
  * [`File`](https://www.arduino.cc/en/Reference/SD)
  * `TwoWire` (the class of [`Wire`](https://www.arduino.cc/en/reference/wire))



`inputSize`: the maximum number of bytes to read from `input`



This function supports two optional parameters:


* a parameter of type `DeserializationOption::NestingLimit` to change the maximum number of nesting levels that the parser will accept (see “Nesting Limit” below);
* a parameter of type `DeserializationOption::Filter` to filter the input document and keep only the fields you need (see “Filtering” below).



## Return value



`deserializeJson()` returns a [`DeserializationError`](https://arduinojson.org/v7/api/misc/deserializationerror/).



## Nesting limit



The ArduinoJson’s parser contains a recursive function that is called each time an opening brace (`{`) or opening bracket (`[`) appears in the input. In other words, each object/array nesting level causes a recursive call.



This recursive call is a security risk because an attacker could craft a JSON input with many opening brackets to cause a stack overflow.



To protect against this security risk, ArduinoJson limits the number of nesting levels. The macro [`ARDUINOJSON_DEFAULT_NESTING_LIMIT`](https://arduinojson.org/v7/config/default_nesting_limit/) sets the default value.



If your JSON input contains more nesting levels than allowed, you can pass an extra parameter of type `DeserializationOption::NestingLimit` to `deserializeJson()`. See the example below.



## Filtering



When the input document contains many fields that are not relevant to your application, you can ask `deserializeJson()` to ignore them and save a lot of space in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



To use this feature, create an ancillary [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) that contains the value `true` as a placeholder for every field you want to keep in the final document. For arrays, only create one element in the filter document, it will serve as a filter for all elements of the original array. Wrap this document in a `DeserializationOption::Filter` before passing it to `deserializeJson()`.



See [JsonFilterExample.ino](https://arduinojson.org/v7/example/filter/) for an example.



## Configuration



`deserializeJson()` can be configured with the following settings:


* [`ARDUINOJSON_DEFAULT_NESTING_LIMIT`](https://arduinojson.org/v7/config/default_nesting_limit/) sets the default nesting limit (see above),
* [`ARDUINOJSON_ENABLE_ARDUINO_STREAM`](https://arduinojson.org/v7/config/enable_arduino_stream/) allows using a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) as input.
* [`ARDUINOJSON_ENABLE_ARDUINO_STRING`](https://arduinojson.org/v7/config/enable_arduino_string/) allows using a [`String`](https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/) as input.
* [`ARDUINOJSON_ENABLE_INFINITY`](https://arduinojson.org/v7/config/enable_infinity/) allows `Infinity` in the input.
* [`ARDUINOJSON_ENABLE_NAN`](https://arduinojson.org/v7/config/enable_nan/) allows `NaN` in the input,
* [`ARDUINOJSON_ENABLE_PROGMEM`](https://arduinojson.org/v7/config/enable_progmem/) allows using a Flash string as input.
* [`ARDUINOJSON_ENABLE_STD_STREAM`](https://arduinojson.org/v7/config/enable_std_stream/) allows using a [`std::istream`](https://en.cppreference.com/w/cpp/io/basic_istream) as input.
* [`ARDUINOJSON_ENABLE_STD_STRING`](https://arduinojson.org/v7/config/enable_std_string/) allows using a [`std::string`](https://en.cppreference.com/w/cpp/string/basic_string) as input.
* [`ARDUINOJSON_ENABLE_STRING_VIEW`](https://arduinojson.org/v7/config/enable_string_view/) allows using a [`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view) as input.



## How to view the JSON input?



When you pass a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to `deserializeJson()`, it consumes the input but doesn’t print anything to the serial, which makes troubleshooting difficult.



If you want to see what `deserializeJson()` consumed, use `ReadLoggingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library. See the example below.



## Performance



When you pass a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to `deserializeJson()`, it consumes bytes one by one, which can be slow depending on the input you use. For example, if you read from a SPIFFS file, you can read twenty times faster by reading chunks of 64 bytes.



To read the stream in chunks, you can use `ReadBufferingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library. See the example below.



## Custom reader



If none of the supported input types is suitable for you, you can implement a custom reader class. This class must implement two member functions, as shown below:



```cpp
struct CustomReader {
  // Reads one byte, or returns -1
  int read();
  // Reads several bytes, returns the number of bytes read.
  size_t readBytes(char* buffer, size_t length);
};
```



Then, pass a reference to an instance of this class as the second argument of [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/).



`deserializeJson()` doesn’t actually use `readBytes()`, so this function is not strictly necessary.
 However, it is used by [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/), and could also be used in future versions of `deserializeJson()`.



## Example



```cpp
const char* json = "{\"hello\":\"world\"}";
JsonDocument doc;
deserializeJson(doc, json);
const char* world = doc["hello"];
```



### Raise the nesting limit



```cpp
const char* json = "[[[[[[[[[[[[[[[42]]]]]]]]]]]]]]]";
JsonDocument doc;
deserializeJson(doc, json, DeserializationOption::NestingLimit(15));;
int answer = doc[0][0][0][0][0][0][0][0][0][0][0][0][0][0][0];
```



### Filtering



```cpp
JsonDocument filter;
filter["list"][0]["dt"] = true;
filter["list"][0]["main"]["temp"] = true;

deserializeJson(doc, input, DeserializationOption::Filter(filter));
```



### Show the JSON input stream



This example requires the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library.



Suppose your program is:



```cpp
deserializeJson(doc, wifiClient);
```



If you want to see what `deserializeJson()` consumed, replace this line with:



```cpp
ReadLoggingStream loggingStream(wifiClient, Serial);
deserializeJson(doc, loggingStream);
```



The first line creates a new [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) on top of `wifiClient` that writes everything it reads to `Serial`.



### Improve read performance



This example requires the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library.



Suppose your program is:



```cpp
deserializeJson(doc, file);
```



If you want to make `deserializeJson()` read chunks instead of reading bytes one by one, replace this line with:



```cpp
ReadBufferingStream bufferingStream(file, 64);
deserializeJson(doc, bufferingStream);
```



The first line creates a new [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) that reads blocks of 64 bytes from `file`.



### Deserialize an object member



Suppose we have the following input:



```cpp
{"config":"{\"user\":\"toto\"}"}
```



If you want to deserialize the JSON document in the `"config"` member, you can do this:



```cpp
JsonDocument doc1, doc2;
deserializeJson(doc1, input);
deserializeJson(doc2, doc1["config"]);
```



After executing these lines, `doc2` contains:



```cpp
{"user":"toto"}
```



## See also


* [Deserialization tutorial](https://arduinojson.org/v7/tutorial/deserialization/)
* [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/)
* [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/)
* [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/)
* [`DeserializationError`](https://arduinojson.org/v7/api/misc/deserializationerror/)


---


# JsonVariant::as<T>()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/as/

## Description



`JsonVariant::as<T>()` casts the value pointed by the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) to the specified type.



Unlike [`JsonVariant::to<T>()`](https://arduinojson.org/v7/api/jsonvariant/to/), this function doesn’t modify the value pointed by the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/).



## Signatures



```cpp
bool               as<bool>() const;

float              as<float>() const;
double             as<double>() const;

signed char        as<signed char>() const;
unsigned char      as<unsigned char>() const;
signed int         as<signed int>() const;
unsigned int       as<unsigned int>() const;
signed short       as<signed short>() const;
unsigned short     as<unsigned short>() const;
signed long        as<signed long>() const;
unsigned long      as<unsigned long>() const;
unsigned long long as<unsigned long long>() const;   // ⚠️ may require ARDUINOJSON_USE_LONG_LONG
signed long long   as<signed long long>() const;     // ⚠️ may require ARDUINOJSON_USE_LONG_LONG

const char*        as<const char*>() const;
String             as<String>() const;               // ⚠️ behavior differs slightly (see below)
std::string        as<std::string>() const;          // ⚠️ behavior differs slightly (see below)

JsonArray          as<JsonArray>() const;
JsonObject         as<JsonObject>() const;
JsonVariant        as<JsonVariant>() const;

JsonArrayConst     as<JsonArrayConst>() const;
JsonObjectConst    as<JsonObjectConst>() const;
JsonVariantConst   as<JsonVariantConst>() const;

TEnum              as<TEnum>() const;  // alias of as<int>()
T                  as<T>() const;      // calls user-defined converter
```



## Return value



`JsonVariant::as<T>()` returns the value pointed by the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) cast to the specified type.



This function returns a default value if the cast is not possible. The default value is:


* `0` for numerical types
* `NULL` for `const char*`
* A null reference for [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) and [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



To change the default value, you need to use [`JsonVariant::operator|`](https://arduinojson.org/v7/api/jsonvariant/or/) instead.



## `as<String>()` and `as<std::string>()`



`JsonVariant::as<T>()` behaves slightly differently when `T` is a string object:


* if the value is a string, it returns this string (nothing special here);
* otherwise, it returns the JSON representation.



Here are a few examples:



| Value | `as<String>()` |
| --- | --- |
| `"hello world"` | `"hello world"` |
| `true` | `"true"` |
| `false` | `"false"` |
| `42` | `"42"` |
| `[1,2,3]` | `"[1,2,3]"` |
| `{"key":"value"}` | `"{\"key\":\"value\"}"` |
| `null` | `"null"` |



If you don’t want this behavior, use `as<const char*>()` instead.



## Integer overflows



`JsonVariant::as<T>()` is aware of integer overflows and only returns a value if it can fit in the specified type.



For example, if the value contains `512`, `as<char>()` returns `0`, but `as<int>()` returns `512`.



## User-defined types



`JsonVariant::as<T>()` supports user-defined types by calling `convertFromJson()`. For example, to support [`tm`](https://en.cppreference.com/w/c/chrono/tm), you must define the following function:



```cpp
void convertFromJson(JsonVariantConst src, tm& dst) {
  strptime(src.as<const char*>(), "%FT%TZ", &dst);
}
```



For more information about custom converters, please read [the article dedicated to ArduinoJson 6.18.0](https://arduinojson.org/news/2021/05/04/version-6-18-0/).



## `as<T>()` vs `to<T>()`



`JsonVariant::as<T>()` and [`JsonVariant::to<T>()`](https://arduinojson.org/v7/api/jsonvariant/to/) look similar but are very different: the former reads the value, whereas the latter changes the reference.



Suppose the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) refers to an object:


* `JsonVariant::as<JsonObject>()` returns a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) pointing to this object.
* [`JsonVariant::to<JsonObject>()`](https://arduinojson.org/v7/api/jsonvariant/to/):
  1. creates a new object in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/),
  2. makes the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) point to the new object,
  3. returns a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) pointing to it.



[`JsonVariant::to<T>()`](https://arduinojson.org/v7/api/jsonvariant/to/) allows **creating an empty object or an empty array**, and it’s probably its only legitimate use. Calling [`JsonVariant::to<T>()`](https://arduinojson.org/v7/api/jsonvariant/to/) with a `T` other than [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) or [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) works but has no practical use.



`JsonVariant::as<T>()`, on the other hand, allows **solving situations where implicit casts don’t work**; for example, when you call a function with several matching overloads (like [`Serial::print()`](https://www.arduino.cc/reference/en/language/functions/communication/serial/print/)) or when you use type deduction with the `auto` keyword (see example below).



Remark that [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/), which is a read-only version of [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), supports [`as<T>()`](https://arduinojson.org/v7/api/jsonvariantconst/as/) but not `to<T>()`.



## Example



```cpp
JsonDocument doc;
deserializeJson(doc, "{\"answer\":42}");

JsonVariant answer = doc["answer"];
auto i = answer.as<int>();         // i == 42
auto d = answer.as<double>();      // d == 42.0
auto s = answer.as<const char*>(); // s == NULL
```



## See also


* [`JsonDocument::as<T>()`](https://arduinojson.org/v7/api/jsondocument/as/)
* [`JsonVariant::is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/)
* [`JsonVariant::operator|`](https://arduinojson.org/v7/api/jsonvariant/or/)
* [`JsonVariant::to<T>()`](https://arduinojson.org/v7/api/jsonvariant/to/)
* [`JsonVariantConst::as<T>()`](https://arduinojson.org/v7/api/jsonvariantconst/as/)


---


# JsonArray
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarray/

## Description



In ArduinoJson, an array is an ordered collection of values. A [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is a reference to an array. The array itself resides in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



Because the [`JsonArray`](https://arduinojson.org/v5/api/jsonarray/) is just a reference, you need a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to create a array.



## Reference semantics



`JsonArray` points to an array in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), giving it reference semantics. Every modification you make through the `JsonArray` is reflected in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). When you copy a `JsonArray`, you only copy the reference, not the array itself.



Don’t pass a `JsonArray` by reference or pointer because it would be a reference to a reference, which is very confusing and can lead to dangling references if you reassign the `JsonArray`.



```cpp
- void setCoordinates(JsonArray& coords) {
+ void setCoordinates(JsonArray coords) {
    JsonDocument doc;
    deserializeJson(doc, dataFile);
-   coords = doc["coordinates"];
+   coords.set(doc["coordinates"]);
  }
```



Avoid returning a `JsonArray` from a function, because it will likely point to a destructed [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). Consider returning a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) instead.



```cpp
- JsonArray getCoordinates() {
+ JsonDocument getCoordinates() {
    JsonDocument doc;
    deserializeJson(doc, dataFile);
    return doc["coordinates"];
  }
```



## Constness



You’ll see that most member functions of [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) are `const`. These methods do not modify the instance, but they may modify the array pointed by the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



As we said, a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is a reference; the `const`-ness of the member functions refers to the reference object, not to the array.



ArduinoJson also supports a read-only reference type named [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/). It’s similar to [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), except it doesn’t allow modifying the array.



## Example



### Create an array and serialize it



```cpp
// create an empty array
JsonDocument doc;
JsonArray array = doc.to<JsonArray>();

// add some values
array.add("hello");
array.add(42);
array.add(3.14);

// serialize the array and send the result to Serial
serializeJson(doc, Serial);
```



### Deserialize an array



```cpp
JsonDocument doc;

// parse a JSON array
deserializeJson(doc, "[1,2,3]");

// extract the values
JsonArray array = doc.as<JsonArray>();
for(JsonVariant v : array) {
    Serial.println(v.as<int>());
}
```



## Member functions


* [`add()`](https://arduinojson.org/v7/api/jsonarray/add/)
* [`begin() / end()`](https://arduinojson.org/v7/api/jsonarray/begin_end/)
* [`clear()`](https://arduinojson.org/v7/api/jsonarray/clear/)
* [`isNull()`](https://arduinojson.org/v7/api/jsonarray/isnull/)
* [`nesting()`](https://arduinojson.org/v7/api/jsonarray/nesting/)
* [`operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/)
* [`remove()`](https://arduinojson.org/v7/api/jsonarray/remove/)
* [`set()`](https://arduinojson.org/v7/api/jsonarray/set/)
* [`size()`](https://arduinojson.org/v7/api/jsonarray/size/)



## See also


* [`JsonDocument::to<JsonArray>()`](https://arduinojson.org/v7/api/jsondocument/to/)
* [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/)
* [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/)
* [Serialization tutorial](https://arduinojson.org/v7/tutorial/serialization/)
* [Deserialization tutorial](https://arduinojson.org/v7/tutorial/deserialization/)


---


# JsonVariant
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/

## Description



`JsonVariant` is a reference to a value in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). It supports all types allowed by JSON:


* boolean
* integer
* floating point
* string
* array
* object



`JsonVariant` doesn’t own the value: it only points to it. The value belongs to the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## Reference semantics



`JsonVariant` points to a value in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), giving it reference semantics. Every modification you make through the `JsonVariant` is reflected in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). When you copy a `JsonVariant`, you only copy the reference, not the value itself.



Don’t pass a `JsonVariant` by reference or pointer because it would be a reference to a reference, which is very confusing and can lead to dangling references if you reassign the `JsonVariant`.



```cpp
- void setHostname(JsonVariant& hostname) {
+ void setHostname(JsonVariant hostname) {
    JsonDocument doc;
    deserializeJson(doc, configFile);
-   hostname = doc["hostname"];
+   hostname.set(doc["hostname"]);
  }
```



Avoid returning a `JsonVariant` from a function, because it will likely point to a destructed [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). Consider returning a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) instead.



```cpp
- JsonVariant getWifiConfig() {
+ JsonDocument getWifiConfig() {
    JsonDocument doc;
    deserializeJson(doc, configFile);
    return doc["wifi"];
  }
```



## Constness



You’ll see that most member functions of `JsonVariant` are `const`. These methods do not modify the instance, but they may alter the value pointed by the `JsonVariant`.



As we said, a `JsonVariant` is a reference; the `const`-ness of the member functions refers to the reference object, not to the value.



ArduinoJson also supports a read-only reference type named [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/). It’s similar to `JsonVariant`, except it doesn’t allow modifying the value.



## Example



### Create a variant and serialize it



```cpp
// create the JsonDocument
JsonDocument doc;

// create a variant
JsonVariant variant = doc.to<JsonVariant>();
variant.set(42);

// serialize the object and send the result to Serial
serializeJson(doc, Serial);
```



### Deserialize a variant



```cpp
// deserialize the value
JsonDocument doc;
deserializeJson(doc, "42");

// extract the data
JsonVariant variant = doc.as<JsonVariant>();
int value = variant.as<int>();
```



## Member functions


* [`add()`](https://arduinojson.org/v7/api/jsonvariant/add/)
* [`as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/)
* [`clear()`](https://arduinojson.org/v7/api/jsonvariant/clear/)
* [`containsKey()`](https://arduinojson.org/v7/api/jsonvariant/containskey/) (deprecated)
* [`isNull()`](https://arduinojson.org/v7/api/jsonvariant/isnull/)
* [`is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/)
* [`nesting()`](https://arduinojson.org/v7/api/jsonvariant/nesting/)
* [`operator|`](https://arduinojson.org/v7/api/jsonvariant/or/)
* [`operator[]`](https://arduinojson.org/v7/api/jsonvariant/subscript/)
* [`remove()`](https://arduinojson.org/v7/api/jsonvariant/remove/)
* [`set()`](https://arduinojson.org/v7/api/jsonvariant/set/)
* [`size()`](https://arduinojson.org/v7/api/jsonvariant/size/)
* [`to<T>()`](https://arduinojson.org/v7/api/jsonvariant/to/)



`JsonVariant` also supports the comparison operators `==`, `!=`, `<`, `<=`, `>`, and `>=`.



## See also


* [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/)
* [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/)
* [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/)


---


# JsonObject
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobject/

## Description



In ArduinoJson, an object is a collection of key-value pairs. A `JsonObject` is a reference to this object, but a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) owns the data.



Because the `JsonObject` is a reference, you need a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to create an object. See the example below.



## Reference semantics



`JsonObject` points to an object in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), giving it reference semantics. Every modification you make through the `JsonObject` is reflected in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). When you copy a `JsonObject`, you only copy the reference, not the object itself.



Don’t pass a `JsonObject` by reference or pointer because it would be a reference to a reference, which is very confusing, and can lead to dangling references if you reassign the `JsonObject`.



```cpp
- void setWifiConfig(JsonObject& cfg) {
+ void setWifiConfig(JsonObject cfg) {
    JsonDocument doc;
    deserializeJson(doc, configFile);
-   cfg = doc["wifi"];
+   cfg.set(doc["wifi"]);
  }
```



Avoid returning a `JsonObject` from a function, because it will likely point to a destructed [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). Consider returning a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) instead.



```cpp
- JsonObject getWifiConfig() {
+ JsonDocument getWifiConfig() {
    JsonDocument doc;
    deserializeJson(doc, configFile);
    return doc["wifi"];
  }
```



## Constness



You’ll see that most member functions of `JsonObject` are `const`. These methods do not modify the instance, but they may alter the object pointed by the `JsonObject`.



As we said, a `JsonObject` is a reference; the `const`-ness of the member functions refers to the reference class, not to the object.



ArduinoJson also supports a read-only reference type named [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/). It’s similar to `JsonObject`, except it doesn’t allow modifying the object.



## `JsonObject` vs `JsonVariant`



Both `JsonObject` and [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) are references to values stored in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



The difference is that [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) can refer to any supported type (integer, float, string, array, object…), whereas `JsonObject` can only refer to an object. The advantage of `JsonObject` over [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is that it supports operations specific to objects, such as [enumerating key-value pairs](https://arduinojson.org/v7/api/jsonobject/begin_end/).



## Example



### Create an object and serialize it



```cpp
JsonDocument doc;

// create an object
JsonObject object = doc.to<JsonObject>();
object["hello"] = "world";

// serialize the object and send the result to Serial
serializeJson(doc, Serial);
```



### Deserialize an object



```cpp
// deserialize the object
JsonDocument doc;
deserializeJson(doc, "{\"hello\":\"world\"}");

// extract the data
JsonObject object = doc.as<JsonObject>();
const char* world = object["hello"];
```



## Member functions


* [`begin() / end()`](https://arduinojson.org/v7/api/jsonobject/begin_end/)
* [`clear()`](https://arduinojson.org/v7/api/jsonobject/clear/)
* [`containsKey()`](https://arduinojson.org/v7/api/jsonobject/containskey/) (deprecated)
* [`isNull()`](https://arduinojson.org/v7/api/jsonobject/isnull/)
* [`nesting()`](https://arduinojson.org/v7/api/jsonobject/nesting/)
* [`operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/)
* [`remove()`](https://arduinojson.org/v7/api/jsonobject/remove/)
* [`set()`](https://arduinojson.org/v7/api/jsonobject/set/)
* [`size()`](https://arduinojson.org/v7/api/jsonobject/size/)



## See also


* [`JsonDocument::as<JsonObject>()`](https://arduinojson.org/v7/api/jsondocument/as/)
* [`JsonDocument::to<JsonObject>()`](https://arduinojson.org/v7/api/jsondocument/to/)
* [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/)
* [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/)
* [Serialization tutorial](https://arduinojson.org/v7/tutorial/serialization/)
* [Deserialization tutorial](https://arduinojson.org/v7/tutorial/deserialization/)


---


# DeserializationError
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/misc/deserializationerror/

## Description



Tells why [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) or [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/) failed.



## Values



A `DeserializationError` is an enumerated type that can contain one of the following values:



### `DeserializationError::Ok`



The deserialization succeeded. Cool!



### `DeserializationError::EmptyInput`



The input was empty or contained only spaces or comments. Possible reasons:


* a timeout occurred when reading from a stream (see [How to change the timeout](https://arduinojson.org/v7/how-to/change-the-timeout/))
* the server returned a [redirection code](https://developer.mozilla.org/en-US/docs/Web/HTTP/Redirections) (like [301 Moved Permanently](https://en.wikipedia.org/wiki/HTTP_301) or [302 Found](https://en.wikipedia.org/wiki/HTTP_302))
* the certificate validation failed (try [`WiFiClientSecure::setInsecure()`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html#setinsecure))
* the stream is not connected
* the file was not found
* the file is empty
* the file was opened in the wrong mode



### `DeserializationError::IncompleteInput`



The end of the input is missing. Possible reasons:


* a buffer was too small to contains the input
* a timeout occurred when reading from a stream (see [How to change the timeout](https://arduinojson.org/v7/how-to/change-the-timeout/))



### `DeserializationError::InvalidInput`



The input is not recognized. Possible reasons:


* the input is simply invalid
* the input contains a comment, but [support is disabled](https://arduinojson.org/v7/config/enable_comments/)
* the input is valid but is preceded by something else (see below)



If this error occurs on an HTTP response, ensure your program:


1. skips the HTTP headers
2. uses HTTP version 1.0 or handles [chunked transfer encoding](https://en.wikipedia.org/wiki/Chunked_transfer_encoding)



See [JsonHttpClient.ino](https://arduinojson.org/v7/example/http-client/) and [How to use ArduinoJson with HTTPClient?](https://arduinojson.org/v7/how-to/use-arduinojson-with-httpclient/).



This error also occurs if the input document starts with a [byte order mark](https://en.wikipedia.org/wiki/Byte_order_mark) (BOM). This problem is hard to diagnose because the BOM is an invisible character, so you can’t see it in the Serial Monitor and in most text editors. The easiest way to check if the BOM is present is to read the first byte of the input (for example, you can do `Serial.print((char)client.read())`). The first character should be a `{` or a `[`; if you see something else, then it’s surely the BOM. If that’s your case, you must skip the first two bytes before calling `deserializeJson()`, like so:



```cpp
client.read();
client.read();
deserializeJson(doc, client);
```



Of course, the best solution is still to remove the BOM from the server side.



### `DeserializationError::NoMemory`



Memory allocation failed.



See also [How to deserialize a very large document?](https://arduinojson.org/v7/how-to/deserialize-a-very-large-document/)



### `DeserializationError::TooDeep`



The nesting limit was reached; you need to increase its value. See [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/), or [`ARDUINOJSON_DEFAULT_NESTING_LIMIT`](https://arduinojson.org/v7/config/default_nesting_limit/) for details.



## Methods



```cpp
// return a string representation of the error
const char* c_str() const;

// same as c_str(), except the string is in Flash memory (only relevant for AVR and ESP8266)
const __FlashStringHelper* f_str() const;

// returns the enum value
Code code() const;
```



## How to know where deserialization stopped?



When you pass a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/), it consumes the input but doesn’t print anything to the serial, which makes troubleshooting difficult.



If you want to see what [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) consumed, use `ReadLoggingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library (see example below). Because ArduinoJson stops reading as soon as it sees an error, you can see what caused the error by checking the last consumed character.



## Example



### Get the error message



```cpp
JsonDocument doc;
DeserializationError err = deserializeJson(doc, "!!NOT JSON!!");
if (err) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(err.c_str());
}
```



the program above prints:



```cpp
deserializeJson() failed: InvalidInput
```



### Switch/case



```cpp
JsonDocument doc;
DeserializationError err = deserializeJson(doc, "!!NOT JSON!!");
switch (err.code()) {
    case DeserializationError::Ok:
        Serial.print(F("Deserialization succeeded"));
        break;
    case DeserializationError::InvalidInput:
        Serial.print(F("Invalid input!"));
        break;
    case DeserializationError::NoMemory:
        Serial.print(F("Not enough memory"));
        break;
    default:
        Serial.print(F("Deserialization failed"));
        break;
}
```



the program above prints:



```cpp
Invalid input!
```



### View the content of the input stream



This example requires the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library.



Suppose the following line returned `InvalidInput`:



```cpp
DeserializationError err = deserializeJson(doc, wifiClient);
```



If you want to see what caused this error, you can make ArduinoJson log the content of the stream by using a `ReadLoggingStream`. Replace the above line with:



```cpp
ReadLoggingStream loggingStream(wifiClient, Serial);
DeserializationError err = deserializeJson(doc, loggingStream);
```



The first line creates a new [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) on top of `wifiClient` that writes everything it reads to `Serial`. Because [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) stops reading as soon as it sees an error, the last character printed to the serial port is the character that triggered the error.



## See also


* [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)
* [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/)
* [Deserialization tutorial](https://arduinojson.org/v7/tutorial/deserialization/)


---


# JsonVariant::operator|
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/or/

## Description



[`JsonVariant::operator|()`](https://arduinojson.org/v7/api/jsonvariant/or/) provides default value for a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/).



## Signatures



```cpp
bool               operator|(bool defaultValue) const;

float              operator|(float defaultValue) const;
double             operator|(double defaultValue) const;

signed char        operator|(signed char defaultValue) const;
unsigned char      operator|(unsigned char defaultValue) const;
signed int         operator|(signed int defaultValue) const;
unsigned int       operator|(unsigned int defaultValue) const;
signed short       operator|(signed short defaultValue) const;
unsigned short     operator|(unsigned short defaultValue) const;
signed long        operator|(signed long defaultValue) const;
unsigned long      operator|(unsigned long defaultValue) const;
unsigned long long operator|(unsigned long long defaultValue) const; // <- may require ARDUINOJSON_USE_LONG_LONG
signed long long   operator|(signed long long defaultValue) const;   // <- may require ARDUINOJSON_USE_LONG_LONG

const char*        operator|(char* defaultValue) const;
const char*        operator|(const char* defaultValue) const;
String             operator|(String defaultValue) const;
std::string        operator|(const std::string& defaultValue) const;
```



## Arguments



`defaultValue`: the value to return if the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is null or incompatible with the requested type.



Unlike [`JsonVariant::as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/) that uses a template parameter to specify the requested type, [`JsonVariant::operator|()`](https://arduinojson.org/v7/api/jsonvariant/or/) infers the requested type from the argument `defaultValue`. Usually, that’s what you want but can lead to odd results as described below.



## Return value


* The value of the variant, if it can be converted to the type of `defaultValue`.
* `defaultValue` if not.



## Example



```cpp
int port = config["port"] | 80;
strlcpy(hostname, config["hostname"] | "example.com", sizeof(hostname));
```



## Pitfall ⚠️



When you use this operator, the type of the expression is inferred from the type of the default value.



Here is an example where this fact matters:



```cpp
long value = root["key"] | -1;
```



In this example, the right side of the expression returns an `int` (and not a `long` as one could expect) because the default value is an integer. If `root["key"]` contains a big value, the integer overflows, and you get a seemingly random result in `value`.



To fix this, you need to change the type of the default value:



```cpp
long value = root["key"] | -1L;
```



Lastly, you cannot combine this operator with [`JsonVariant::as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/). For example, you cannot write:



```cpp
// this line compiles but it doesn't do what you think
long value = root["key"].as<long>() | 1; // WRONG!!!!
```



## See also


* [`JsonVariant::as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/)
* [`JsonVariant::is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/)
* [`JsonVariantConst::operator|`](https://arduinojson.org/v7/api/jsonvariantconst/or/)
* [`JsonConfigFile.ino`](https://arduinojson.org/v7/example/config/)


---


# JsonVariant::add()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/add/

## Description



When the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) points to an array, `JsonVariant::add()` appends a value to the array. Otherwise, this function does nothing.



In other words, `JsonVariant::add()` reproduces [`JsonArray::add()`](https://arduinojson.org/v7/api/jsonarray/add/).



## Signatures



```cpp
bool add(bool value) const;

bool add(float value) const;
bool add(double value) const;

bool add(signed char value) const;
bool add(signed long value) const;
bool add(signed int value) const;
bool add(signed short value) const;
bool add(unsigned char value) const;
bool add(unsigned long value) const;
bool add(unsigned int value) const;
bool add(unsigned short value) const;

bool add(const char (&value)[N]) const;            // stores a pointer
bool add(const char *value) const;                 // stores a copy
bool add(const __FlashStringHelper *value) const;  // stores a copy

bool add(const String &value) const;       // stores a copy
bool add(const std::string &value) const;  // stores a copy
bool add(const Printable& value) const;    // stores a copy
bool add(std::string_view value) const;    // stores a copy

bool add(JsonArray array) const;          // stores a deep copy
bool add(JsonObject object) const;        // stores a deep copy
bool add(JsonVariant variant) const;      // stores a deep copy
bool add(const JsonDocument& doc) const;  // stores a deep copy

bool add(TEnum value) const;  // alias of add(int)
bool add(T value) const;      // calls user-defined function

JsonArray   add<JsonArray>() const;    // adds a new empty array
JsonObject  add<JsonObject>() const;   // adds a new empty object
JsonVariant add<JsonVariant>() const;  // adds a new null variant
```



## Arguments



`value`: the value to append to the array.



All values are stored by copy, except string literals which are stored by address.



The string copy policy has changed in [ArduinoJson 7.3](https://arduinojson.org/news/2024/12/29/arduinojson-7-3/).



## Return value



`JsonVariant::add()` returns a `bool` that tells whether the operation was successful or not:


* `true` if the value was successfully added.
* `false` if there was not enough memory in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## User-defined types



`JsonVariant::add()` supports user-defined types by calling `convertToJson()`. For example, to support [`tm`](https://en.cppreference.com/w/c/chrono/tm), you must define:



```cpp
bool convertToJson(const tm& src, JsonVariant dst) {
  char buf[32];
  strftime(buf, sizeof(buf), "%FT%TZ", &src);
  return dst.set(buf);
}
```



## See also


* [`JsonArray::add()`](https://arduinojson.org/v7/api/jsonarray/add/)
* [`JsonDocument::add()`](https://arduinojson.org/v7/api/jsondocument/add/)


---


# JsonVariant::to<T>()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/to/

## Description



`JsonVariant::to<T>()` clears the content of the variant and changes its type to `T`.



It has no effect if the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is unbound.



## Signatures



```cpp
JsonArray  to<JsonArray>() const;
JsonObject to<JsonObject>() const;
```



## Return value



A reference to the new value of the variant.



## `as<T>()` vs `to<T>()`



[`JsonVariant::as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/) and `JsonVariant::to<T>()` look similar but are very different: the former reads the value, whereas the latter changes the reference.



Suppose the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) refers to an object:


* [`JsonVariant::as<JsonObject>()`](https://arduinojson.org/v7/api/jsonvariant/as/) returns a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) pointing to this object.
* `JsonVariant::to<JsonObject>()`:
  1. creates a new object in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/),
  2. makes the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) point to the new object,
  3. returns a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) pointing to it.



`JsonVariant::to<T>()` allows **creating an empty object or an empty array**, and it’s probably its only legitimate use. Calling `JsonVariant::to<T>()` with a `T` other than [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) or [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) works but has no practical use.



[`JsonVariant::as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/), on the other hand, allows **solving situations where implicit casts don’t work**; for example, when you call a function with several matching overloads (like [`Serial::print()`](https://www.arduino.cc/reference/en/language/functions/communication/serial/print/)) or when you use type deduction with the `auto` keyword (see example below).



Remark that [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/), which is a read-only version of [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), supports [`as<T>()`](https://arduinojson.org/v7/api/jsonvariantconst/as/) but not `to<T>()`.



## Example



```cpp
void fillConfig(JsonVariant variant) {
  JsonObject config = variant.to<JsonObject>();
  config["host"] = "127.0.0.1";
  config["port"] = 80;
}
```



## See also


* [`JsonVariant::as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/)
* [`JsonDocument::to<T>()`](https://arduinojson.org/v7/api/jsondocument/to/)


---


# serializeJson()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/json/serializejson/

## Description



[`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) serializes a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to create a *minified* JSON document, i.e. a document without spaces or line break between values.



If you want a *prettified* JSON document, use [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/)



## Signatures



```cpp
size_t serializeJson(TSource src, char* output, size_t outputSize);  // see remark 1
size_t serializeJson(TSource src, char output[N]);                   // see remark 1
size_t serializeJson(TSource src, Print& output);
size_t serializeJson(TSource src, String& output);
size_t serializeJson(TSource src, std::string& output);
size_t serializeJson(TSource src, std::ostream& output);

template<typename Writer> // custom writer class (see below)
size_t serializeJson(TSource src, Writer& output);
```



**➀** These overloads only add the null terminator if there is enough room in the buffer; this is the same caveat as `strncpy()`.
 Therefore, make sure that the buffer is large enough or check the return value.



The two first overloads support `unsigned char` as well.



## Arguments


* `src`: the value to serialize. It can be a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/), [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), or [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/).
* `output`: the destination buffer where the result should be written
* `outputSize`: the capacity of the destination buffer



Because `output` can be any implementation of [`Print`](https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Print.h), you can uses instances like [`Serial`](https://www.arduino.cc/reference/en/language/functions/communication/serial/), [`EthernetClient`](https://www.arduino.cc/en/Reference/EthernetClient), [`WifiClient`](https://www.arduino.cc/en/Reference/WiFiClient)…



## Return value



The number of bytes written, excluding the null terminator (`char*` and `char[N]` overloads only).



## Configuration



You can configure `serializeJson()` with the following settings:


* [`ARDUINOJSON_ENABLE_NAN`](https://arduinojson.org/v7/config/enable_nan/) writes `NaN` instead of `null`,
* [`ARDUINOJSON_ENABLE_INFINITY`](https://arduinojson.org/v7/config/enable_infinity/) writes `Infinity` instead of `null`.



## How to view the JSON output?



When you pass a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/), it writes the JSON to the stream but doesn’t print anything to the serial port, which makes troubleshooting difficult.



If you want to see what [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) writes, use `WriteLoggingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library. See the example below.



## Performance



When you pass a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/), it sends the bytes one by one, which can be slow depending on the target stream. For example, if you send to a `WiFiClient` on an [ESP8266](https://en.wikipedia.org/wiki/ESP8266), it will send a packet over the air for each byte, which is terribly slow and inefficient. To improve speed and efficiency, we must send fewer, larger packets.



To write the JSON document in chunks, you can use `WriteBufferingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library. See the example below.



## Custom writer



If none of the supported output types is suitable for you, you can implement a custom writer class. This class must implement two member functions, as shown below:



```cpp
struct CustomWriter {
  // Writes one byte, returns the number of bytes written (0 or 1)
  size_t write(uint8_t c);
  // Writes several bytes, returns the number of bytes written
  size_t write(const uint8_t *buffer, size_t length);
};
```



Then, pass a reference to an instance of this class as the second argument of [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/).



## Example



### Write the JSON document to the serial port



```cpp
JsonDocument doc;
doc["hello"] = "world";
serializeJson(doc, Serial);
```



will write the following string to the serial port:



```cpp
{"hello":"world"}
```



### Write to a file



```cpp
File file = SD.open(filename, FILE_WRITE);
JsonDocument doc;
doc["hello"] = "world";
serializeJson(doc, file);
file.close();
```



### Write to a file and print to the serial port



This example requires the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library; it uses the `WriteLoggingStream` decorator that adds logging capability to an existing [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/).



```cpp
File file = SD.open(filename, FILE_WRITE);
WriteLoggingStream loggedFile(file, Serial);
doc["hello"] = "world";
serializeJson(doc, loggedFile);
file.close();
```



The second line create a new [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) that forward every calls to `file` and duplicate writes calls to `Serial`.



### Improve file write performance



This example requires the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library; it uses the `WriteBufferingStream` decorator that adds buffering capability to an existing [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/).



```cpp
File file = SD.open(filename, FILE_WRITE);
WriteBufferingStream bufferedFile(file, 64);
doc["hello"] = "world";
serializeJson(doc, bufferedFile);
bufferedFile.flush();
file.close();
```



The first line creates the file and the second line adds a 64 byte buffer in front of the file. The next to last line writes the remaining bytes in the file.



## See also


* [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/)
* [`measureJson()`](https://arduinojson.org/v7/api/json/measurejson/)
* [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/)
* [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)


---


# serializeJsonPretty()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/json/serializejsonpretty/

## Description



Serializes the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to create a prettified JSON document, i.e. a document with spaces and line-breaks between values.



If you want a “minified” JSON document, use [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/)



## Signatures



```cpp
size_t serializeJsonPretty(TSource, char* output, size_t outputSize);  // see remark 1
size_t serializeJsonPretty(TSource, char output[N]);                   // see remark 1
size_t serializeJsonPretty(TSource, Print& output);
size_t serializeJsonPretty(TSource, String& output);
size_t serializeJsonPretty(TSource, std::string& output);

template<typename Writer> // custom writer class (see below)
size_t serializeJsonPretty(TSource, Writer& output);
```



**➀** These overloads only add the null terminator if there is enough room in the buffer; this is the same caveat as `strncpy()`.
 Therefore, make sure that the buffer is large enough or check the return value.



The two first overloads support `unsigned char` as well.



## Arguments


* `src`: the value to serialize. It can be a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/), [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), or [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/).
* `output`: the destination buffer where the result should be written
* `outputSize`: the capacity of the destination buffer



Because `output` can be any implementation of [`Print`](https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Print.h), you can uses instances like [`Serial`](https://www.arduino.cc/reference/en/language/functions/communication/serial/), [`EthernetClient`](https://www.arduino.cc/en/Reference/EthernetClient), [`WifiClient`](https://www.arduino.cc/en/Reference/WiFiClient)…



## Return value



The number of bytes written, excluding the null terminator (`char*` and `char[N]` overloads only).



## Configuration



You can configure `serializeJsonPretty()` with the following settings:


* [`ARDUINOJSON_ENABLE_NAN`](https://arduinojson.org/v7/config/enable_nan/) writes `NaN` instead of `null`,
* [`ARDUINOJSON_ENABLE_INFINITY`](https://arduinojson.org/v7/config/enable_infinity/) writes `Infinity` instead of `null`.
* `ARDIUNOJSON_TAB` is the string used to indent the document (default is `"  "`, i.e., two spaces)



## How to view the JSON output?



When you pass a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/), it writes the JSON to the stream but doesn’t print anything to the serial port, which makes troubleshooting difficult.



If you want to see what [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/) writes, use `WriteLoggingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library. See the example below.



## Performance



When you pass a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/), it sends the bytes one by one, which can be slow depending on the target stream. For example, if you send to a `WiFiClient` on an [ESP8266](https://en.wikipedia.org/wiki/ESP8266), it will send a packet over the air for each byte, which is terribly slow and inefficient. To improve speed and efficiency, we must send fewer, larger packets.



To write the JSON document in chunks, you can use `WriteBufferingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library. See the example in [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/)



## Custom writer



If none of the supported output types is suitable for you, you can implement a custom writer class. This class must implement two member functions, as shown below:



```cpp
struct CustomWriter {
  // Writes one byte, returns the number of bytes written (0 or 1)
  size_t write(uint8_t c);
  // Writes several bytes, returns the number of bytes written
  size_t write(const uint8_t *buffer, size_t length);
};
```



Then, pass a reference to an instance of this class as the second argument of [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/).



## Example



```cpp
JsonDocument doc;
doc["hello"] = "world";
serializeJsonPretty(doc, Serial);
```



will write the following string to the serial output:



```cpp
{
  "hello": "world"
}
```



## See also


* [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/)
* [`measureJsonPretty()`](https://arduinojson.org/v7/api/json/measurejsonpretty/)
* [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/)
* [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)


---


# measureJson()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/json/measurejson/

## Description



Computes the length of the minified JSON document that [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) produces, excluding the null-terminator.



This function is handy to fill the `Content-Length` header in an HTTP message.



## Signatures



```cpp
size_t measureJson(TSource src);
```



## Arguments


* `src`: the value to serialize. It can be a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/), [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), or [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/).



## Return value



The number of characters that [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) produces, excluding the null-terminator.



## Example



```cpp
// Send headers
client.println("Content-Type: application/json");
client.print("Content-Length: ");
client.println(measureJson(doc));

// Terminate headers
client.println();

// Send body
serializeJson(doc, client);
```



## See also


* [`measureJsonPretty()`](https://arduinojson.org/v7/api/json/measurejsonpretty/)
* [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/)


---


# measureJsonPretty()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/json/measurejsonpretty/

## Description



Computes the length of the prettified JSON document that [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/) produces, excluding the null-terminator.



This function is handy to fill the `Content-Length` header of an HTTP message.



## Signature



```cpp
size_t measureJsonPretty(TSource src);
```



## Arguments


* `src`: the value to serialize. It can be a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/), [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), or [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/).



## Return value



The number of characters that [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/)produces, excluding the null-terminator.



## Example



```cpp
// Send headers
client.println("Content-Type: application/json");
client.print("Content-Length: ");
client.println(measureJsonPretty(doc));

// Terminate headers
client.println();

// Send body
serializeJsonPretty(doc, client);
```



## See also


* [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/)
* [`measureJson()`](https://arduinojson.org/v7/api/json/measurejson/)


---


# deserializeMsgPack()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/msgpack/deserializemsgpack/

## Description



`deserializeMsgPack()` parses a MessagePack input and puts the result in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



Before reading the input, this function resets the document, so you don’t need to call [`JsonDocument::clear()`](https://arduinojson.org/v7/api/jsondocument/clear/).



## Signatures



```cpp
DeserializationError deserializeMsgPack(JsonDocument& doc, const char* input);
DeserializationError deserializeMsgPack(JsonDocument& doc, const char* input, size_t inputSize);
DeserializationError deserializeMsgPack(JsonDocument& doc, const __FlashStringHelper* input);
DeserializationError deserializeMsgPack(JsonDocument& doc, const __FlashStringHelper* input, size_t inputSize);
DeserializationError deserializeMsgPack(JsonDocument& doc, const String& input); // 💀
DeserializationError deserializeMsgPack(JsonDocument& doc, const std::string& input);
DeserializationError deserializeMsgPack(JsonDocument& doc, Stream& input);
DeserializationError deserializeMsgPack(JsonDocument& doc, std::istream& input);
DeserializationError deserializeMsgPack(JsonDocument& doc, JsonVariantConst input);

template<typename Reader> // custom reader class (see below)
DeserializationError deserializeMsgPack(JsonDocument& doc, Reader& input);

// all overloads also accept optional parameters (see below)
```



Arduino’s `String` doesn’t allow nulls inside the string; don’t use this class for MessagePack documents.



## Arguments



`doc`: the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) that will store the memory representation of the MessagePack document.



`input`: the MessagePack document to parse:


* `const char*` is a string in RAM
* `const __FlashStringHelper*` is a Flash string, usually created with `F()`
* [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) is Arduino’s I/O stream interface, implemented by:
  * `HardwareSerial` (the class of [`Serial`](https://www.arduino.cc/reference/en/language/functions/communication/serial/))
  * [`SoftwareSerial`](https://www.arduino.cc/en/Reference/SoftwareSerial)
  * [`EthernetClient`](https://www.arduino.cc/en/Reference/EthernetClient)
  * [`WifiClient`](https://www.arduino.cc/en/Reference/WiFiClient)
  * `WiFiClientSecure`
  * `BluetoothClient`
  * [`EthernetUDP`](https://www.arduino.cc/en/Reference/EthernetUDPBegin)
  * [`GSMClient`](https://www.arduino.cc/en/Reference/GSM)
  * [`File`](https://www.arduino.cc/en/Reference/SD)
  * `TwoWire` (the class of [`Wire`](https://www.arduino.cc/en/reference/wire))



`inputSize`: the maximum number of bytes to read from `input`



This function supports two optional parameters:


* a parameter of type `DeserializationOption::NestingLimit` to change the maximum number of nesting levels that the parser will accept (see “Nesting Limit” below);
* a parameter of type `DeserializationOption::Filter` to filter the input document and keep only the fields you need (see “Filtering” below).



## Return value



`deserializeMsgPack()` returns a [`DeserializationError`](https://arduinojson.org/v7/api/misc/deserializationerror/).



## Configuration



`deserializeMsgPack()` can be configured with the following settings:


* [`ARDUINOJSON_DEFAULT_NESTING_LIMIT`](https://arduinojson.org/v7/config/default_nesting_limit/) sets the default nesting limit (see above),
* [`ARDUINOJSON_ENABLE_ARDUINO_STREAM`](https://arduinojson.org/v7/config/enable_arduino_stream/) allows using a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) as input.
* [`ARDUINOJSON_ENABLE_ARDUINO_STRING`](https://arduinojson.org/v7/config/enable_arduino_string/) allows using a [`String`](https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/) as input (not recommended).
* [`ARDUINOJSON_ENABLE_INFINITY`](https://arduinojson.org/v7/config/enable_infinity/) allows `Infinity` in the input.
* [`ARDUINOJSON_ENABLE_NAN`](https://arduinojson.org/v7/config/enable_nan/) allows `NaN` in the input,
* [`ARDUINOJSON_ENABLE_PROGMEM`](https://arduinojson.org/v7/config/enable_progmem/) allows using a Flash string as input.
* [`ARDUINOJSON_ENABLE_STD_STREAM`](https://arduinojson.org/v7/config/enable_std_stream/) allows using a [`std::istream`](https://en.cppreference.com/w/cpp/io/basic_istream) as input.
* [`ARDUINOJSON_ENABLE_STD_STRING`](https://arduinojson.org/v7/config/enable_std_string/) allows using a [`std::string`](https://en.cppreference.com/w/cpp/string/basic_string) as input.



## Nesting limit



The ArduinoJson’s parser contains a recursive function that is called each time an object or an array begins. In other words, each object/array nesting level causes a recursive call. This recursive call is a security risk because an attacker could craft a JSON input with many opening brackets to cause a stack overflow.



To protect against this security risk, ArduinoJson limits the number of nesting levels. The macro [`ARDUINOJSON_DEFAULT_NESTING_LIMIT`](https://arduinojson.org/v7/config/default_nesting_limit/) sets the default value.



If your MessagePack input contains more nesting levels than allowed, you can pass an extra parameter of type `DeserializationOption::NestingLimit` to `deserializeMsgPack()`.



## Filtering



When the input document contains many fields that are not relevant to your application, you can ask `deserializeMsgPack()` to ignore them and save a lot of space in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



To use this feature, create an ancillary [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) that contains the value `true` as a placeholder for every field you want to keep in the final document. For arrays, only create one element in the filter document, it will serve as a filter for all elements of the original array. Wrap this document in a `DeserializationOption::Filter` before passing it to `deserializeMsgPack()`.



See [JsonFilterExample.ino](https://arduinojson.org/v7/example/filter/) for an example.



## Performance



When you pass a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to `deserializeMsgPack()`, it consumes bytes one by one, which can be slow depending on the input you use. For example, if you read from a SPIFFS file, you can read twenty times faster by reading chunks of 64 bytes.



To read the stream in chunks, you can use `ReadBufferingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library.



Suppose your program is:



```cpp
deserializeMsgPack(doc, file);
```



If you want to make `deserializeMsgPack()` read chunks instead of reading bytes one by one, replace this line with:



```cpp
ReadBufferingStream bufferingStream(file, 64);
deserializeMsgPack(doc, bufferingStream);
```



The first line creates a new [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) that reads blocks of 64 bytes from `file`.



## Custom reader



If none of the supported input types is suitable for you, you can implement a custom reader class. This class must implement two member functions, as shown below:



```cpp
struct CustomReader {
  // Reads one byte, or returns -1
  int read();
  // Reads several bytes, returns the number of bytes read.
  size_t readBytes(char* buffer, size_t length);
};
```



Then, pass a reference to an instance of this class as the second argument of [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/).



## Example



```cpp
char input[] = "\x81\xA5hello\xA5world";
JsonDocument doc;
deserializeMsgPack(doc, input);
const char* world = doc["hello"];
```



## See also


* [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/)
* [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)
* [`DeserializationError`](https://arduinojson.org/v7/api/misc/deserializationerror/)
* [Comparative benchmark of `serializeJson()` vs `serializeMsgPack()`](https://github.com/fabianoriccardi/benchmark-json-messagepack)


---


# serializeMsgPack()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/msgpack/serializemsgpack/

## Description



[`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/) serializes a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to create a [MessagePack](https://msgpack.org) document.



## Signatures



```cpp
size_t serializeMsgPack(TSource src, char* output, size_t outputSize);
size_t serializeMsgPack(TSource src, char output[size]);
size_t serializeMsgPack(TSource src, Print& output);
size_t serializeMsgPack(TSource src, String& output);  // 💀
size_t serializeMsgPack(TSource src, std::string& output);

template<typename Writer> // custom writer class (see below)
size_t serializeMsgPack(TSource src, Writer& output);
```



Arduino’s `String` doesn’t allow nulls inside the string; don’t use this class for MessagePack documents.



The two first overloads support `unsigned char` as well.



## Arguments


* `src`: the value to serialize. It can be a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/), [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), or [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/).
* `output`: the destination buffer where the result should be written
* `outputSize`: the capacity of the destination buffer



Because `output` can be any implementation of [`Print`](https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Print.h), you can uses instances like [`Serial`](https://www.arduino.cc/reference/en/language/functions/communication/serial/), [`EthernetClient`](https://www.arduino.cc/en/Reference/EthernetClient), [`WifiClient`](https://www.arduino.cc/en/Reference/WiFiClient)…



## Return value



[`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/) returns the number of bytes written.



## Performance



When you pass a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/), it writes in small chunks, which can be slow depending on the target stream. For example, if you send to a `WiFiClient` on an [ESP8266](https://en.wikipedia.org/wiki/ESP8266), it will send many packets over the air, which is slow and inefficient. To improve speed and efficiency, we must send fewer, larger packets.



To write the MessagePack document in larger chunks, you can use `WriteBufferingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library. See the example in [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/).



## Custom writer



If none of the supported output types is suitable for you, you can implement a custom writer class. This class must implement two member functions, as shown below:



```cpp
struct CustomWriter {
  // Writes one byte, returns the number of bytes written (0 or 1)
  size_t write(uint8_t c);
  // Writes several bytes, returns the number of bytes written
  size_t write(const uint8_t *buffer, size_t length);
};
```



Then, pass a reference to an instance of this class as the second argument of [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/).



## Example



```cpp
JsonDocument doc;
doc["hello"] = "world";
serializeMsgPack(doc, Serial);
```



will write the following bytes to the serial output:



```cpp
81 A5 68 65 6C 6C 6F A5 77 6F 72 6C 64
```



## See also


* [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/)
* [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/)
* [`measureMsgPack()`](https://arduinojson.org/v7/api/msgpack/measuremsgpack/)
* [Comparative benchmark of `serializeJson()` vs `serializeMsgPack()`](https://github.com/fabianoriccardi/benchmark-json-messagepack)


---


# measureMsgPack()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/msgpack/measuremsgpack/

## Description



[`measureMsgPack()`](https://arduinojson.org/v7/api/msgpack/measuremsgpack/) computes the length of the [MessagePack](https://msgpack.org) document that [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/) produces.



This function is handy to fill the `Content-Length` header in an HTTP message.



## Signature



```cpp
size_t measureMsgPack(TSource src);
```



## Arguments


* `src`: the value to serialize. It can be a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/), [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), or [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/).



## Return value



The number of bytes that [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/) produces.



## Example



```cpp
// Send headers
client.println("Content-Type: application/msgpack");
client.print("Content-Length: ");
client.println(measureMsgPack(doc));

// Terminate headers
client.println();

// Send body
serializeMsgPack(doc, client);
```



## See also


* [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/)
* [`measureJson()`](https://arduinojson.org/v7/api/json/measurejson/)


---


# MsgPackBinary
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/msgpack/msgpackbinary/

## Description



The `MsgPackBinary` class allows you to read or write MessagePack binary data.



This feature was added in ArduinoJson 7.1.0.



Internally, `MsgPackBinary` uses the same storage as [`serialized()`](https://arduinojson.org/v7/api/misc/serialized/) values.



## Constructors



```cpp
MsgPackBinary(const void* data, size_t size);
```



## Member functions



```cpp
const void* data() const;
size_t size() const;
```



## Example



### Serialization



```cpp
char buffer[] = {1, 2, 3};

JsonDocument doc;
doc['data'] = MsgPackBinary(buffer, sizeof(buffer));
serializeMsgPack(doc, output);   // 81 A4 65 61 74 61 C4 03 01 02 03
```



`MsgPackBinary` doesn’t hold a copy of the data; it only stores a pointer and a size. Therefore, you must ensure the buffer remains valid until you insert it into a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) (where it will be copied).



### Deserialization



```cpp
JsonDocument doc;
deserializeMsgPack(doc, input);

MsgPackBinary data = doc['data'];
// or: auto data = doc['data'].as<MsgPackBinary>();

const void* buffer = data.data();
size_t size = data.size();
```



`MsgPackBinary` doesn’t hold a copy of the data; it only stores a pointer and a size. Therefore, you must ensure the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) remains valid as long as the `MsgPackBinary` object is in use.



## Size restrictions



[`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) stores the size of the binary data in an integer whose width is determined by [`ARDUINOJSON_STRING_LENGTH_SIZE`](https://arduinojson.org/v7/config/string_length_size/).



With the default settings, the maximum size of the binary data is:



| CPU architecture | Max binary size |
| --- | --- |
| 8-bit | 253 |
| 32-bit | 65,632 |
| 64-bit | 65,632 |



## See also


* [`MsgPackExtension`](https://arduinojson.org/v7/api/msgpack/msgpackextension/)
* [`serialized()`](https://arduinojson.org/v7/api/misc/serialized/)
* [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/)
* [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/)


---


# MsgPackExtension
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/msgpack/msgpackextension/

## Description



The `MsgPackExtension` class allows you to read or write MessagePack extension data.



This feature was added in ArduinoJson 7.1.0.



Internally, `MsgPackExtension` uses the same storage as [`serialized()`](https://arduinojson.org/v7/api/misc/serialized/) values.



## Constructors



```cpp
MsgPackExtension(int8_t type, const void* data, size_t size);
```



## Member functions



```cpp
int8_t type() const;
const void* data() const;
size_t size() const;
```



## Example



### Serialization



```cpp
char buffer[] = {1, 2, 3};

JsonDocument doc;
doc['data'] = MsgPackExtension(4, buffer, sizeof(buffer));
serializeMsgPack(doc, output);   // 81 A4 65 61 74 61 C7 03 04 01 02 03
```



`MsgPackExtension` doesn’t hold a copy of the data; it only stores a pointer and a size. Therefore, you must ensure the buffer remains valid until you insert it into a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) (where it will be copied).



### Deserialization



```cpp
JsonDocument doc;
deserializeMsgPack(doc, input);

MsgPackExtension data = doc['data'];
// or: auto data = doc['data'].as<MsgPackExtension>();

int8_t type = data.type();
const void* buffer = data.data();
size_t size = data.size();
```



`MsgPackExtension` doesn’t hold a copy of the data; it only stores a pointer and a size. Therefore, you must ensure the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) remains valid as long as the `MsgPackExtension` object is in use.



## Size restrictions



[`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) stores the size of the binary data in an integer whose width is determined by [`ARDUINOJSON_STRING_LENGTH_SIZE`](https://arduinojson.org/v7/config/string_length_size/).



With the default settings, the maximum size of the extension data is:



| CPU architecture | Max extension size |
| --- | --- |
| 8-bit | 252 |
| 32-bit | 65,631 |
| 64-bit | 65,631 |



## See also


* [`MsgPackBinary`](https://arduinojson.org/v7/api/msgpack/msgpackbinary/)
* [`serialized()`](https://arduinojson.org/v7/api/misc/serialized/)
* [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/)
* [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/)


---


# ARDUINOJSON_VERSION
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/misc/version/

## Description



ArduinoJson defines several macros to allow a program to detect the version of the library.



There are four macros:


* `ARDUINOJSON_VERSION` is a string that is typically used to display the version to the user
* `ARDUINOJSON_VERSION_MAJOR` contains the major version number
* `ARDUINOJSON_VERSION_MINOR` contains the minor version number
* `ARDUINOJSON_VERSION_REVISION` contains the revision number



For example, ArduinoJson 7.0.0-beta defines the following:



```cpp
#define ARDUINOJSON_VERSION "7.0.0-beta"
#define ARDUINOJSON_VERSION_MAJOR 7
#define ARDUINOJSON_VERSION_MINOR 0
#define ARDUINOJSON_VERSION_REVISION 0
```



## How to use?



### How to detect that ArduinoJson is available?



```cpp
#ifndef ARDUINOJSON_VERSION
#error ArduinoJson not found, please include ArduinoJson.h in your .ino file
#endif
```



### How to verify that the right version is installed?



```cpp
#if ARDUINOJSON_VERSION_MAJOR != 7
#error ArduinoJson 7 is required
#endif
```



### How to display ArduinoJson version?



```cpp
Serial.print("Using ArduinoJson version ");
Serial.println(ARDUINOJSON_VERSION);
```


---


# copyArray()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/misc/copyarray/

## Description



[`copyArray()`](https://arduinojson.org/v7/api/misc/copyarray/) copies values between a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) (or [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)) and a regular C array.



It works in both directions:


* from a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) (or [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)) to a regular array,
* from a regular array to a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) (or [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/))



It supports both 1D and 2D arrays.



## Signatures



```cpp
// In the following lines T can be any type supported by JsonVariant
// (for example: bool, int, float, or const char*).
// N, N1, and N2 are integer constants.

// 1D array -> JsonArray/JsonDocument
bool copyArray(T[N] src, JsonArray dst);
bool copyArray(T[N] src, JsonDocument& dst);

// 1D array -> JsonArray/JsonDocument
bool copyArray(T* src, size_t len, JsonArray dst);
bool copyArray(T* src, size_t len, JsonDocument& dst);

// 2D array -> JsonArray/JsonDocument
bool copyArray(T[N1][N2] src, JsonArray dst);
bool copyArray(T[N1][N2] src, JsonDocument& dst);

// JsonArray/JsonDocument -> 1D array
size_t copyArray(JsonArrayConst src, T[N] dst);
size_t copyArray(const JsonDocument& src, T[N] dst);

// JsonArray/JsonDocument -> 1D array
size_t copyArray(JsonArrayConst src, T* dst, size_t len);
size_t copyArray(const JsonDocument& src, T* dst, size_t len);

// JsonArray -> 2D array
void copyArray(JsonArrayConst src, T[N1][N2] dst);
void copyArray(const JsonDocument& src, T[N1][N2] dst);

// these function also support JsonDocument
```



## Return value



Some overloads of [`copyArray()`](https://arduinojson.org/v7/api/misc/copyarray/) return a `bool` that tells whether the copy was successful.



Some overloads of [`copyArray()`](https://arduinojson.org/v7/api/misc/copyarray/) return a `size_t` that contains the number of elements copied.



## Example



### 1D array



```cpp
int values[] = {1,2,3};

JsonDocument doc;
copyArray(values, doc);
serializeJson(doc, Serial);
```



will write the following string to the serial port:



```cpp
[1,2,3]
```



### 2D array



```cpp
int values[][3] = {{1, 2, 3}, {4, 5, 6}};

JsonDocument doc;
copyArray(values, doc);
serializeJson(doc, Serial);
```



will write the following string to the serial port:



```cpp
[[1,2,3],[4,5,6]]
```


---


# JsonString
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonstring/

## Description



`JsonString` is an object that refers to a constant string in RAM. You can see it a a supercharged `const char*` that supports:


* comparison operations
* non-zero-terminated strings
* [NUL characters](https://en.wikipedia.org/wiki/Null_character) inside the string



It is similar to [`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view).



## Interface



```cpp
class JsonString {
 public:
  JsonString(const char* p=0, bool isStatic=false);
  JsonString(const char* p, size_t n, bool isStatic=false);

  size_t size() const;
  const char* c_str() const;
  bool isStatic() const;
};
```



[ArduinoJson 7.3](https://arduinojson.org/news/2024/12/29/arduinojson-7-3/): the `isStatic` flag indicates that the string is safe to store by address.



## See also


* [`JsonObject::begin()` / `JsonObject::end()`](https://arduinojson.org/v7/api/jsonobject/begin_end/)


---


# serialized()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/misc/serialized/

## Description



[`serialized()`](https://arduinojson.org/v7/api/misc/serialized/) allows you to insert preformatted chunks of JSON (or MessagePack) in a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) or a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



Unlike with regular string value, ArduinoJson doesn’t escape the special characters when the string is marked as “serialized.”



You can use this function when you have a part of the document that never changes; you can hard-code this part in the source code, to optimize the serialization.



You can also use this function to insert a part that you cannot recreate with the library.



## Signatures



```cpp
SerializedValue serialized(const char *value);
SerializedValue serialized(const char *value, size_t size);
SerializedValue serialized(char *value);
SerializedValue serialized(char *value, size_t size);
SerializedValue serialized(const String &value);
SerializedValue serialized(const std::string &value);
SerializedValue serialized(std::string_view value);
SerializedValue serialized(const __FlashStringHelper *value);
SerializedValue serialized(const __FlashStringHelper *value, size_t size);
```



## Return value



[`serialized()`](https://arduinojson.org/v7/api/misc/serialized/) returns a type that is recognized by the library as a raw value that should be serialized as is.



## Example



```cpp
JsonDocument doc;
doc["hello"] = serialized("[\"wor\",\"ld!\"]");
serializeJson(doc, Serial);
```



will write the following string to the serial port:



```cpp
{"hello":["wor","ld!"]}
```


---


# How to upgrade from ArduinoJson 6 to 7


Source: https://arduinojson.org/v7/how-to/upgrade-from-v6/

As you’ll see, ArduinoJson’s API hasn’t changed much between version 6 and 7, and your existing programs should compile with some deprecation warnings. Even if your code compiles, you’re strongly encouraged to update it to leverage the simplifications and improvements of ArduinoJson 7.



On this page, you’ll find concise instructions to upgrade your code. You’ll find more information about the underlying changes and explanations about the design decisions in [the dedicated news article](https://arduinojson.org/news/2024/01/03/arduinojson-7/).



> > ##### ArduinoJson 7 is significantly bigger ⚠️



> ArduinoJson 6 had a strong focus on code size because 8-bit microcontrollers were still dominant at the time. ArduinoJson 7 loosened the code size constraint to focus on ease of use. As a result, version 7 is significantly bigger than version 6.



> If your program targets 8-bit microcontrollers, I recommend **keeping version 6**.



## `JsonDocument`



In ArduinoJson 6, you could choose between [`StaticJsonDocument`](https://arduinojson.org/v6/api/staticjsondocument/) and [`DynamicJsonDocument`](https://arduinojson.org/v6/api/dynamicjsondocument/); the former being (most likely) allocated on the stack and the latter on the heap.



In ArduinoJson 7, a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)s always allocates its memory on the heap, so [`StaticJsonDocument`](https://arduinojson.org/v6/api/staticjsondocument/) and [`DynamicJsonDocument`](https://arduinojson.org/v6/api/dynamicjsondocument/) were merged into a single [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) class.



Also, the new [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) has an elastic capacity that automatically grows as required, so you don’t need to specify the capacity anymore.



```cpp
- StaticJsonDocument<256> doc;
+ JsonDocument doc;
```



```cpp
- DynamicJsonDocument doc(256);
+ JsonDocument doc;
```



## `capacity()`



Now that [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) grows as needed, there is no such thing as a “document capacity” anymore, so [`JsonDocument::capacity()`](https://arduinojson.org/v6/api/jsondocument/capacity/) has been removed.



We often called this function to check that the memory pool of a [`DynamicJsonDocument`](https://arduinojson.org/v6/api/dynamicjsondocument/) was allocated correctly. Now, you should use [`JsonDocument::overflowed()`](https://arduinojson.org/v7/api/jsondocument/overflowed/) instead:



```cpp
- if (doc.capacity() == 0)
-   Serial.println("ERROR: allocation failed!");
  doc["hello"] = "value";
  // ...
+ if (doc.overflowed())
+   Serial.println("ERROR: no enough memory to store the entire document");
```



## `createNestedArray()` and `createNestedObject()`



In ArduinoJson 6, [`createNestedArray()`](https://arduinojson.org/v6/api/jsonvariant/createnestedarray/) and [`createNestedObject()`](https://arduinojson.org/v6/api/jsonvariant/createnestedobject/) allowed adding a new array or object to an existing array or object.
 In ArduinoJson 7, these functions are replaced with [`add<T>()`](https://arduinojson.org/v7/api/jsonvariant/add/) and [`to<T>()`](https://arduinojson.org/v7/api/jsonvariant/to/).



#### Combination 1: array in array



To create an array in an array, you must call [`add<JsonArray>()`](https://arduinojson.org/v7/api/jsonvariant/add/), like so:



```cpp
- JsonArray coords = locations.createNestedArray();
+ JsonArray coords = locations.add<JsonArray>();
  coords.add(lat);
  coords.add(long);
```



#### Combination 2: object in array



To create an object in an array, you must call [`add<JsonObject>()`](https://arduinojson.org/v7/api/jsonvariant/add/), like so:



```cpp
- JsonObject phineas = doc.createNestedObject();
+ JsonObject phineas = doc.add<JsonObject>();
  phineas["first"] = "Phineas";
  phineas["last"] = "Flynn";
- JsonObject ferb = doc.createNestedObject();
+ JsonObject ferb = doc.add<JsonObject>();
  ferb["first"] = "Ferb";
  ferb["last"] = "Fletcher";
```



#### Combination 3: array in object



To create an array in an object, you must call [`operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/) followed by [`to<JsonArray>()`](https://arduinojson.org/v7/api/jsonvariant/to/), like so:



```cpp
- JsonArray ports = doc.createNestedArray("ports");
+ JsonArray ports = doc["ports"].to<JsonArray>();
  ports.add(80);
  ports.add(443);
```



#### Combination 4: object in object



To create an object in an object, you must call [`operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/) followed by [`to<JsonObject>()`](https://arduinojson.org/v7/api/jsonvariant/to/), like so:



```cpp
- JsonObject location = doc.createNestedObject("location");
+ JsonObject location = doc["location"].to<JsonObject>();
  location["lat"] = lat;
  location["long"] = long;
```



## `containsKey()`



In ArduinoJson 6, [`containsKey()`](https://arduinojson.org/v6/api/jsonobject/containskey/) checked if an object contained a specific key.
 In ArduinoJson 7, you must use [`operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/) followed by [`is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/).



```cpp
- if (doc.containsKey("key")) {
+ if (doc["key"].is<int>()) {
    int value = doc["key"];
    // ...
  }
```



This syntax not only checks that the key exists but also that the value is of the expected type. It was already available in ArduinoJson 6.



## `memoryUsage()`



In ArduinoJson 6, [`JsonDocument::memoryUsage()`](https://arduinojson.org/v6/api/jsondocument/memoryusage/) told you how many bytes were used in the [`JsonDocument`](https://arduinojson.org/v6/api/jsondocument/).



We used this function to determine the correct capacity for a [`JsonDocument`](https://arduinojson.org/v6/api/jsondocument/). The elastic capacity renders this function useless.



```cpp
- Serial.println(doc.memoryUsage());
```



## `garbageCollect()`



In ArduinoJson 7, [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) reuses released memory, so [`garbageCollect()`](https://arduinojson.org/v6/api/jsondocument/garbagecollect/) has been removed.



```cpp
- doc.garbageCollect();
```



## `shallowCopy()`



In ArduinoJson 6, [`JsonVariant::shallowCopy()`](https://arduinojson.org/v6/api/jsonvariant/shallowcopy/) allowed you to store a pointer to a variant from another [`JsonDocument`](https://arduinojson.org/v6/api/jsondocument/).



Due to a significant change in ArduinoJson 7, it’s not possible to store a pointer anymore, so [`shallowCopy()`](https://arduinojson.org/v6/api/jsonvariant/shallowcopy/) has been removed.



```cpp
- doc1["key"].shallowCopy(doc2);
+ doc1["key"] = doc2;
```



Of course, it now makes a deep copy of the variant, and you’ll have to refactor your code if you want to avoid this copy. For example, instead of returning a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) from a function, you could pass a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) for the function to fill:



```cpp
- DynamicJsonDocument getCredentials() {
+ void getCredentials(JsonObject creds) {
-   DynamicJsonDocument creds(1024);
    creds["username"] = "candace"
    creds["password"] = "bu$t3d!";
-   return creds;
  }

- DynamicJsonDocument config(1024);
+ JsonDocument config;
- DynamicJsonDocument credentials = getCredentials();
- config["credentials"].shallowCopy(credentials);
+ getCredentials(config["credentials"].to<JsonObject>());
```



Admittedly, it would be possible to recreate [`shallowCopy()`](https://arduinojson.org/v6/api/jsonvariant/shallowcopy/) in ArduinoJson 7, but it would require dedicated plumbing in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) class, and I don’t think it’s worth the additional code size and complexity.



## `shrinkToFit()`



[`JsonDocument::shrinkToFit()`](https://arduinojson.org/v7/api/jsondocument/shrinktofit/) is still available and releases the over-allocated memory, just like [`std::string::shrink_to_fit()`](https://en.cppreference.com/w/cpp/string/basic_string/shrink_to_fit). However, it is now automatically called by [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/), so it’s very unlikely that you need to call it yourself.



## Custom allocator



In ArduinoJson 6, you could specify a custom allocator class as a template parameter of [`BasicJsonDocument`](https://arduinojson.org/v6/api/basicjsondocument/).
 In ArduinoJson 7, you must inherit from [`ArduinoJson::Allocator`](https://arduinojson.org/v7/api/jsondocument/#allocator) and pass a pointer to an instance of your class to the constructor of [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



```cpp
- struct SpiRamAllocator {
+ struct SpiRamAllocator : ArduinoJson::Allocator {
  void* allocate(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};

- BasicJsonDocument<SpiRamAllocator> doc(4096);
+ SpiRamAllocator allocator;
+ JsonDocument doc(&allocator);
```



Note that the `reallocate()` method is now required.



## `JSON_ARRAY_SIZE()` and `JSON_OBJECT_SIZE()`



In ArduinoJson 6, the macros `JSON_ARRAY_SIZE()` and `JSON_OBJECT_SIZE()` were used to compute the capacity to store arrays and objects in a [`JsonDocument`](https://arduinojson.org/v6/api/jsondocument/). In ArduinoJson 7, there is no need to specify the capacity anymore, so these macros have been removed.



```cpp
- StaticJsonDocument<JSON_OBJECT_SIZE(2)> doc;
+ JsonDocument doc;
```



## `serializeJson()` and `String`



In ArduinoJson 6, [`serializeJson()`](https://arduinojson.org/v6/api/json/serializejson/) appended the JSON document to a [`String`](https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/), which has always been a source of confusion.
 In ArduinoJson 7, [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) replaces the content of the [`String`](https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/).



```cpp
- outputString = "";
  serializeJson(doc, outputString);
```



The same applies to [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/), [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/), and [`std::string`](https://en.cppreference.com/w/cpp/string/basic_string).



## See also


* [Upgrading from ArduinoJson 5 to 7](https://arduinojson.org/v7/how-to/upgrade-from-v5/)


---


# How to upgrade from ArduinoJson 5 to 7


Source: https://arduinojson.org/v7/how-to/upgrade-from-v5/

ArduinoJson has changed a lot since version 5, so the upgrade process is quite complicated. I recommend that you read the whole document before starting and then proceed step by step.



> > ##### ArduinoJson 7 is significantly bigger ⚠️



> ArduinoJson 5 had a strong focus on code size because 8-bit microcontrollers were largely dominant at the time. ArduinoJson 7 loosened the code size constraint to focus on ease of use. As a result, version 7 is significantly bigger than version 5.



> If your program targets 8-bit microcontrollers, I recommend **upgrading to version 6 instead**, which size is comparable to version 5.



## References



In ArduinoJson 5, [`JsonArray`](https://arduinojson.org/v5/api/jsonarray/) and [`JsonObject`](https://arduinojson.org/v5/api/jsonobject/) were always returned by reference to emphasize that they reside in the [`JsonBuffer`](https://arduinojson.org/v5/api/jsonbuffer/).
 In ArduinoJson 7, [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), and [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) are smart pointers, so they are returned by value.



```cpp
- JsonObject& obj = ...
+ JsonObject obj = ...
```



```cpp
- JsonArray& arr = ...
+ JsonArray arr = ...
```



Don’t be fooled by the fact that they are returned by value: they don’t contain a copy of the array/object but a pointer to the original one located in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) (see next item).



## `JsonDocument`



With ArduinoJson 5, it was very difficult to use a [`JsonObject`](https://arduinojson.org/v5/api/jsonobject/) or a [`JsonArray`](https://arduinojson.org/v5/api/jsonarray/) as a class member because you had to ensure that the [`JsonBuffer`](https://arduinojson.org/v5/api/jsonbuffer/) also stayed in memory. The trick was to add the [`JsonBuffer`](https://arduinojson.org/v5/api/jsonbuffer/) as a class member too, but it was more complicated than it should be.



ArduinoJson 7 replaces the concept of [`JsonBuffer`](https://arduinojson.org/v5/api/jsonbuffer/) with the concept of [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). The [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) owns the memory and contains the root of the object tree. You can see a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) as a combination of [`JsonBuffer`](https://arduinojson.org/v5/api/jsonbuffer/) and [`JsonVariant`](https://arduinojson.org/v5/api/jsonvariant/). Also, there is no need to specify the capacity anymore.



```cpp
- StaticJsonBuffer<200> jsonBuffer;
+ JsonDocument doc;
```



```cpp
- DynamicJsonBuffer jsonBuffer(200);
+ JsonDocument doc;
```



## Automatic conversion



In ArduinoJson 5, you had to call either [`JsonBuffer::createArray()`](https://arduinojson.org/v5/api/jsonbuffer/createarray/) or [`JsonBuffer::createObject()`](https://arduinojson.org/v5/api/jsonbuffer/createobject/) to create an empty array or object.
 In ArduinoJson 7, you don’t need to choose because the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) automatically switches to the right type (array or object) according to the way you use it.



For example, in the following snippet, the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) implicitly converts to an object:



```cpp
JsonDocument doc;
doc["hello"] = "world";
// doc contains {"hello":"world"}
```



Whereas in the next snippet, it converts to an array:



```cpp
JsonDocument doc;
doc.add("hello");
doc.add("world");
// doc contains ["hello","world"]
```



This automatic conversion occurs when the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is empty, but you can also force a conversion by calling [`to<T>()`](https://arduinojson.org/v7/api/jsondocument/to/):



```cpp
JsonDocument doc;
JsonObject obj = doc.to<JsonObject>();
obj["hello"] = "world";
```



This can be useful when you need to create an empty object or array.



## `deserializeJson()`



In ArduinoJson 5, you invoked the JSON parser by calling [`JsonBuffer::parseObject()`](https://arduinojson.org/v5/api/jsonbuffer/parseobject/) or [`JsonBuffer::parseArray()`](https://arduinojson.org/v5/api/jsonbuffer/parsearray/).
 In ArduinoJson 7, you call the function [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) and pass the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) and input as arguments.



```cpp
- JsonObject& obj = jb.parseObject(input);
+ deserializeJson(doc, input);
```



Each time you call [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/), it clears the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), so you can reuse a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) several times, which was not possible with [`JsonBuffer`](https://arduinojson.org/v5/api/jsonbuffer/).



## `DeserializationError`



In ArduinoJson 5, you used [`JsonObject::success()`](https://arduinojson.org/v5/api/jsonobject/success/) or [`JsonArray::success()`](https://arduinojson.org/v5/api/jsonarray/success/) to check whether the parsing succeeded, and you had no information on what went wrong.
 In ArduinoJson 7, you can look at the [`DeserializationError`](https://arduinojson.org/v7/api/misc/deserializationerror/) returned by [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/). You can test individual values like `DeserializationError::InvalidInput` or `DeserializationError::NoMemory`, or you can simply convert the error to a string by calling `.c_str()`.



```cpp
- JsonObject& obj = jb.parseObject(input);
+ DeserializationError error = deserializeJson(doc, input);
- if (!obj.success()) {
+ if (error) {
-   Serial.println("parseObject() failed");
+   Serial.print("deserializeJson() returned ");
+   Serial.println(error.c_str());
    return;
  }
```



## `createNestedArray()` and `createNestedObject()`



In ArduinoJson 5, [`createNestedArray()`](https://arduinojson.org/v5/api/jsonobject/createnestedarray/) and [`createNestedObject()`](https://arduinojson.org/v5/api/jsonobject/createnestedobject/) allowed adding a new array or object in an existing array or object.
 In ArduinoJson 7, these functions are replaced with [`add<T>()`](https://arduinojson.org/v7/api/jsonvariant/add/) and [`to<T>()`](https://arduinojson.org/v7/api/jsonvariant/to/).



For example, to create an array of objects, you could write:



```cpp
- JsonObject& phineas = root.createNestedObject();
+ JsonObject phineas = doc.add<JsonObject>();
  phineas["first"] = "Phineas";
  phineas["last"] = "Flynn";
- JsonObject& ferb = root.createNestedObject();
+ JsonObject ferb = doc.add<JsonObject>();
  ferb["first"] = "Ferb";
  ferb["last"] = "Fletcher";
```



And to create an array in an object, you could write:



```cpp
- JsonArray& ports = doc.createNestedArray("ports");
+ JsonArray ports = doc["ports"].to<JsonArray>();
  ports.add(80);
  ports.add(443);
```



## `containsKey()`



In ArduinoJson 5, [`containsKey()`](https://arduinojson.org/v5/api/jsonobject/containskey/) checked if an object contained a specific key.
 In ArduinoJson 7, you must use [`operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/) followed by [`is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/).



```cpp
- if (obj.containsKey("key")) {
+ if (obj["key"].is<int>()) {
    int value = obj["key"];
    // ...
  }
```



This syntax not only checks that the key exists but also that the value is of the expected type. It was already available in ArduinoJson 5.



## `serializeJson()` and `serializeJsonPretty()`



In ArduinoJson 5, when you wanted to serialize a [`JsonArray`](https://arduinojson.org/v5/api/jsonarray/) or a [`JsonObject`](https://arduinojson.org/v5/api/jsonobject/), you called [`JsonArray::printTo()`](https://arduinojson.org/v5/api/jsonarray/printto/) or [`JsonObject::printTo()`](https://arduinojson.org/v5/api/jsonobject/printto/).
 In ArduinoJson 7, you call the function [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) and pass it the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



```cpp
- obj.printTo(Serial);
+ serializeJson(doc, Serial);
```



Similarly, you can call [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/) to produce a prettified JSON document.



## `measureJson()` and `measureJsonPretty()`



With ArduinoJson 5, you could compute the length of the serialized document by calling [`JsonArray::measureLength()`](https://arduinojson.org/v5/api/jsonarray/measurelength/) or [`JsonObject::measureLength()`](https://arduinojson.org/v5/api/jsonobject/measurelength/).
 With ArduinoJson 7, you call [`measureJson()`](https://arduinojson.org/v7/api/json/measurejson/) to do that.



```cpp
- size_t len = obj.measureLength();
+ size_t len = measureJson(doc);
```



Similarly, [`measureJsonPretty()`](https://arduinojson.org/v7/api/json/measurejsonpretty/) replaces [`JsonArray::measurePrettyLength()`](https://arduinojson.org/v5/api/jsonarray/measureprettylength/) and [`JsonObject::measureJsonPretty()`](https://arduinojson.org/v5/api/jsonobject/measureprettylength/).



## Nesting limit



In ArduinoJson 5, you could change the nesting limit by passing an optional argument to [`JsonBuffer::parseArray()`](https://arduinojson.org/v5/api/jsonbuffer/parsearray/) or [`JsonBuffer::parseObject()`](https://arduinojson.org/v5/api/jsonbuffer/parseobject/).
 In ArduinoJson 7, you must pass this value to [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) and cast it to `DeserializationOption::NestingLimit`:



```cpp
- JsonObject& obj = jb.parseObject(input, 20);
+ deserializeJson(doc, input, DeserializationOption::NestingLimit(20));
```



## `serialized()`



In ArduinoJson 5, when you wanted to insert a preformatted piece of JSON, you called [`RawJson()`](https://arduinojson.org/v5/api/utils/rawjson/).
 In ArduinoJson 7, you call [`serialized()`](https://arduinojson.org/v7/api/misc/serialized/):



```cpp
- obj["raw"] = RawJson("[1,2,3]");
+ doc["raw"] = serialized("[1,2,3]");
```



## `JsonPair`



In ArduinoJson 5, when you enumerated the members of a [`JsonObject`](https://arduinojson.org/v5/api/jsonobject/), you received a `JsonPair` with two member variables: `key` and `value`. The first was a `const char*` and the second a [`JsonVariant`](https://arduinojson.org/v5/api/jsonvariant/).



In ArduinoJson 7, `JsonPair::key()` and `JsonPair::value()` are member functions. Also, `key` doesn’t return a `const char*` but a [`JsonString`](https://arduinojson.org/v7/api/jsonstring/), so you must call `JsonString::c_str()` to get the pointer.



```cpp
  for (JsonPair p : obj) {
  for (JsonPair p : obj) {
-   const char* key = p.key;
+   const char* key = p.key().c_str();
-   JsonVariant value = p.value;
+   JsonVariant value = p.value();
    ...
  }
```



## `copyArray()`



In ArduinoJson 5, you could easily copy values between a [`JsonArray`](https://arduinojson.org/v5/api/jsonarray/) and a C array using [`JsonArray::copyFrom()`](https://arduinojson.org/v5/api/jsonarray/copyfrom/) and [`JsonArray::copyTo()`](https://arduinojson.org/v5/api/jsonarray/copyto/).
 In ArduinoJson 7, you must call [`copyArray()`](https://arduinojson.org/v7/api/misc/copyarray/) instead. There is only one function for both operations. The first argument is the source, and the second is the destination.



```cpp
  int values[] = {1,2,3};

- arr.copyFrom(values);
+ copyArray(values, arr);

- arr.copyTo(values);
+ copyArray(arr, values);
```



## `JsonVariant` is a reference



In ArduinoJson 5, [`JsonVariant`](https://arduinojson.org/v5/api/jsonvariant/) had value semantic, and you could create an instance without a [`JsonBuffer`](https://arduinojson.org/v5/api/jsonbuffer/).
 In ArduinoJson 7, [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) has reference semantics, like [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) and [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), and you need a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to create one.



```cpp
- JsonVariant var = 42;
+ JsonDocument doc;
+ JsonVariant var = doc.to<JsonVariant>();
+ var.set(42);
```



But you could also set the value directly on the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/):



```cpp
- JsonVariant var = 42;
+ JsonDocument doc;
+ doc.set(42);
```



## `isNull()`



In ArduinoJson 5, you checked if an array or an object was valid by calling [`success()`](https://arduinojson.org/v5/api/jsonvariant/success/).
 In ArduinoJson 7, you use [`isNull()`](https://arduinojson.org/v7/api/jsonvariant/isnull/) instead:



```cpp
- if (!obj.success()) ...
+ if (obj.isNull()) ...
```



[`isNull()`](https://arduinojson.org/v7/api/jsonvariant/isnull/) is not the exact opposite of [`success()`](https://arduinojson.org/v5/api/jsonvariant/success/): when the value is defined but is `null`, both [`isNull()`](https://arduinojson.org/v7/api/jsonvariant/isnull/) and [`success()`](https://arduinojson.org/v5/api/jsonvariant/success/) return `true`.



Also, note that, in ArduinoJson 5, [`is<const char*>()`](https://arduinojson.org/v5/api/jsonvariant/is/) returned `true` if the value was `null`; it’s no longer the case in version 7.



Lastly, there is also a conversion to boolean, which returns `false` if the value of the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), or [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is `null` or `false`. So, in most cases, you can simply write:



```cpp
- if (!obj.success()) ...
+ if (!obj) ...
```



## Naked `char`



ArduinoJson 7 no longer supports the `char` type, so you must replace it with either `signed char`, `unsigned char`, `int8_t`, `uint8_t`, or any other integral type.



```cpp
- char age = obj["age"];
+ int8_t age = doc["age"];
```



```cpp
- auto height = obj["height"].as<char>();
+ auto height = doc["height"].as<int8_t>();
```



## Non-const `char*`



ArduinoJson 7 doesn’t support non-const `char*`, so you must always use `const char*`.



```cpp
- Serial.println(obj["msg"].as<char*>());
+ Serial.println(doc["msg"].as<const char*>());
```



## Summary



### Deserialization



```cpp
// ArduinoJson 5
DynamicJsonBuffer jb;
JsonObject& obj = jb.parseObject(json);
if (!obj.success())
  return;
int value = obj["value"];
```



```cpp
// ArduinoJson 7
JsonDocument doc;
DeserializationError error = deserializeJson(doc, json);
if (error)
  return;
int value = doc["value"];
```



### Serialization



```cpp
// ArduinoJson 5
DynamicJsonBuffer jb;
JsonObject& obj = jb.createObject();
obj["key"] = "value";
obj["raw"] = RawJson("[1,2,3]");
obj.printTo(Serial);
```



```cpp
// ArduinoJson 7
JsonDocument doc;
doc["key"] = "value";
doc["raw"] = serialized("[1,2,3]");
serializeJson(doc, Serial);
```



## See also


* [How to upgrade from ArduinoJson 5 to 6](https://arduinojson.org/v6/doc/upgrade/)
* [How to upgrade from ArduinoJson 6 to 7](https://arduinojson.org/v7/how-to/upgrade-from-v6/)


---


# How to merge JSON objects?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/merge-json-objects/

This page explains how to write a function that merges two JSON objects into one.



Before using this function, please remember that ArduinoJson is a *serialization* library. As such, it is designed to serialize and deserialize JSON documents, not to store the state of an application.



## Shallow merge



Suppose you have two JSON objects:



```cpp
{
  "name": "Benoit"
}
```



and



```cpp
{
  "age": 38
}
```



And suppose that your goal is to merge the two objects into one:



```cpp
{
  "name": "Benoit",
  "age": 38
}
```



Well, you can do that easily with the following function:



```cpp
void merge(JsonObject dest, JsonObjectConst src)
{
   for (JsonPairConst kvp : src)
   {
     dest[kvp.key()] = kvp.value();
   }
}
```



See online demo on wandbox.org.



## Deep merge



The `merge()` function above only merges the top-level object. It blindly overrides members of the destination object.



Now, let’s see how we can merge the members instead of overriding them.



For example, imagine you have this object:



```cpp
{
  "config": {
    "wifi": {
      "ssid": "thebatcave"
    },
    "light": {
      "state": "off"
    }
  }
}
```



And suppose you want to merge with the following object:



```cpp
{
  "config": {
    "wifi": {
      "pass": "i'm*batman!"
    }
  }
}
```



With a deep merging function, this would give:



```cpp
{
  "config": {
    "wifi": {
      "ssid": "thebatcave",
      "pass": "i'm*batman!"
    },
    "light": {
      "state": "off"
    }
  }
}
```



Here an example implementation for this function:



```cpp
void merge(JsonVariant dst, JsonVariantConst src)
{
  if (src.is<JsonObjectConst>())
  {
    for (JsonPairConst kvp : src.as<JsonObjectConst>())
    {
      if (dst[kvp.key()])
        merge(dst[kvp.key()], kvp.value());
      else
        dst[kvp.key()] = kvp.value();
    }
  }
  else
  {
    dst.set(src);
  }
}
```



See online demo on wandbox.org.



This function uses:


* [`JsonVariant::is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/)
* [`JsonVariant::as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/)
* [`JsonObject::begin()/end()`](https://arduinojson.org/v7/api/jsonobject/begin_end/)
* [`JsonVariant::set()`](https://arduinojson.org/v7/api/jsonvariant/set/)


---


# How to list the keys in a JsonDocument?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/list-keys-in-a-jsondocument/

You can enumerate keys in a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) with [iterators](https://arduinojson.org/v7/api/jsonobject/begin_end/). However, if you have a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), you first need to cast it to a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



For example, here is how you would print all the keys at the root of a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/):



```cpp
for (JsonPair kv : doc.as<JsonObject>()) {
  Serial.println(kv.key().c_str());
}
```



You could also get the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) associated with each key with `kv.value()`.



Check out the “Recursive Analyzer” case study in the last chapter of [Mastering ArduinoJson](https://arduinojson.org/book/) to see how you can not only print the root keys and types, but also the nested objects recursively.


---


# How to recursively search for a key?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/search-nested-key/

The function below recursively searches for a key in a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/). It allows you to find a key in nested objects.



```cpp
JsonVariant findNestedKey(JsonObject obj, const char* key) {
    JsonVariant foundObject = obj[key];
    if (!foundObject.isNull())
        return foundObject;

    for (JsonPair pair : obj) {
        JsonVariant nestedObject = findNestedKey(pair.value(), key);
        if (!nestedObject.isNull())
            return nestedObject;
    }

    return JsonVariant();
}
```



For example, suppose you have this JSON document:



```cpp
{
  "europe": {
    "france": {
      "paris": 2165423,
      "marseille": 870731,
    },
    "italy": {
      "rome": 2844395,
      "milan": 3368590,
    },
  }
}
```



You could use this function below to find the population of Paris, like so:



```cpp
unsigned long parisPopulation = findNestedKey(doc.as<JsonObject>(), "paris");
```



In addition, if you want to support constant references, you need to use this overload:



```cpp
JsonVariantConst findNestedKey(JsonObjectConst obj, const char* key) {
    JsonVariantConst foundObject = obj[key];
    if (!foundObject.isNull())
        return foundObject;

    for (JsonPairConst pair : obj) {
        JsonVariantConst nestedObject = findNestedKey(pair.value(), key);
        if (!nestedObject.isNull())
            return nestedObject;
    }

    return JsonVariantConst();
}
```



### See also:


* [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) / [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/)
* [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) / [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/)
* [`JsonDocument::as<T>()`](https://arduinojson.org/v7/api/jsondocument/as/)


---


# How to add support for char?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/add-support-for-char/

C++ supports three types of `char`: `signed char`, `unsigned char`, and “naked” `char`.
 `signed char` and `unsigned char` are integer types, but “naked” `char` is a character type.



JSON supports numbers and strings but has no “character” type.
 If you want to store an integer, you can use `signed char`, `unsigned char`, or another integer type.
 If you want to store a string, you can use `const char*`, `String`, or any supported string type.



Old versions of ArduinoJson used to treat “naked” `char` as an integer type, but this caused a few problems.
 [ArduinoJson 6.18](https://arduinojson.org/news/2021/05/04/version-6-18-0/#support-for-naked-char-removed) deprecated the support for “naked” `char`, and then [ArduinoJson 6.20](https://arduinojson.org/news/2022/12/26/arduinojson-6-20-0/#removed-support-for-char-and-char) removed it entirely.



You can write a [custom converter](https://arduinojson.org/news/2021/05/04/version-6-18-0/) to restore the support for “naked” `char`, but you must choose between two options: treat `char` as an integer or a character type.



## Option 1: treat `char` as an integer type



The following converter treats `char` as `signed char`. Note that `char` can be `signed` or `unsigned`, depending on the compiler, so you may need to adapt this code.



```cpp
namespace ArduinoJson {
template <>
struct Converter<char> {
  static void toJson(char c, JsonVariant var) {
    var.set(static_cast<signed char>(buf));
  }

  static char fromJson(JsonVariantConst src) {
    return static_cast<char>(var.as<signed char>());
  }

  static bool checkJson(JsonVariantConst src) {
    return src.is<signed char>();
  }
};
}
```



## Option 2: treat `char` as a character type



The following converter stores `char`s as one-character strings.



```cpp
namespace ArduinoJson {
template <>
struct Converter<char> {
  static void toJson(char c, JsonVariant var) {
    char buf[] = {c, 0};  // create a string of length 1
    var.set(buf);
  }

  static char fromJson(JsonVariantConst src) {
    auto p = src.as<const char*>();
    return p ? p[0] : 0;  // returns the first character or 0
  }

  static bool checkJson(JsonVariantConst src) {
    auto p = src.as<const char*>();
    return p && p[0] && !p[1];  // must be a string of length 1
  }
};
}
```


---


# How to create converters for STL containers?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/create-converters-for-stl-containers/

[Custom converters](https://arduinojson.org/news/2021/05/04/version-6-18-0/) allow ArduinoJson to support user-defined types. This page shows how we can write converters for STL containers.



I’ll probably include these converters in the library someday.



## `std::vector`



The following class adds support for `std::vector<T>`. Of course, `T` itself has to be supported by ArduinoJson, either natively or through a custom converter.



```cpp
namespace ArduinoJson {
template <typename T>
struct Converter<std::vector<T> > {
  static void toJson(const std::vector<T>& src, JsonVariant dst) {
    JsonArray array = dst.to<JsonArray>();
    for (T item : src)
      array.add(item);
  }

  static std::vector<T> fromJson(JsonVariantConst src) {
    std::vector<T> dst;
    for (T item : src.as<JsonArrayConst>())
      dst.push_back(item);
    return dst;
  }

  static bool checkJson(JsonVariantConst src) {
    JsonArrayConst array = src;
    bool result = array;
    for (JsonVariantConst item : array)
      result &= item.is<T>();
    return result;
  }
};
}  // namespace ArduinoJson
```



Thanks to this converter, you can now write thinks like this:



```cpp
std::vector<int> v1 = {1, 2};
doc["values"] = v1;

if (doc["values"].is<std::vector<int> >()) {
  std::vector<int> v2 = doc["values"];
  // ...
}
```



The same technique works with `std::list`. If you’re a TMP wizard, you can write a generic implementation by leveraging the `Enable` template parameter of `Converter`.



## `std::array`



For `std::array`, we need a different converter because:


1. it doesn’t support the `push_back()` operation
2. it has a fixed size, so `checkJson()` must check it



Here is a possible implementation:



```cpp
namespace ArduinoJson {
template <typename T, size_t N>
struct Converter<std::array<T, N> > {
  static void toJson(const std::array<T, N>& src, JsonVariant dst) {
    JsonArray array = dst.to<JsonArray>();
    for (T item : src)
      array.add(item);
  }

  static std::array<T, N> fromJson(JsonVariantConst src) {
    std::array<T, N> dst;
    size_t idx = 0;
    for (T item : src.as<JsonArrayConst>())
      dst[idx++] = item;
    return dst;
  }

  static bool checkJson(JsonVariantConst src) {
    JsonArrayConst array = src;
    bool result = array;
    size_t size = 0;
    for (JsonVariantConst item : array) {
      result &= item.is<T>();
      size++;
    }
    return result && size == N;
  }
};
}  // namespace ArduinoJson
```



## `std::map`



Here is the implementation for `std::map<std::string, T>`, you can use a similar implementation for `std::unordered_map`.



```cpp
namespace ArduinoJson {
template <typename T>
struct Converter<std::map<std::string, T> > {
  static void toJson(const std::map<std::string, T>& src, JsonVariant dst) {
    JsonObject obj = dst.to<JsonObject>();
    for (const auto& item : src)
      obj[item.first] = item.second;
  }

  static std::map<std::string, T> fromJson(JsonVariantConst src) {
    std::map<std::string, T> dst;
    for (JsonPairConst item : src.as<JsonObjectConst>())
      dst[item.key().c_str()] = item.value().as<T>();
    return dst;
  }

  static bool checkJson(JsonVariantConst src) {
    JsonObjectConst obj = src;
    bool result = obj;
    for (JsonPairConst item : obj)
      result &= item.value().is<T>();
    return result;
  }
};
}  // namespace ArduinoJson
```


---


# How to create converters for IPAddress?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/create-converters-for-ipaddress/

On this page, we’ll see how we can create a [custom converter](https://arduinojson.org/news/2021/05/04/version-6-18-0/) so you can directly insert or retrieve instances of `IPAddress` like so:



```cpp
// Insert into a JsonDocument
doc["ip"] = IPAddress(192, 168, 0, 1);

// Retrieve from a JsonDocument
IPAddress ip = doc["ip"];
```



In this scenario, the JSON document looks like this:



```cpp
{"ip":"192.168.0.1"}
```



## Converting `IPAddress` to JSON



Inserting an `IPAddress` into a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) works out-of-the-box because `IPAddress` implements the `Printable` interface (and ArduinoJson has supported this interface since [version 6.18.0](https://arduinojson.org/news/2021/05/04/version-6-18-0/)).



Therefore, you don’t need to write any code.



## Converting JSON to `IPAddress`



You must declare the following function:



```cpp
void convertFromJson(JsonVariantConst src, IPAddress& dst) {
  dst.fromString(src.as<const char*>());
}
```



As you can see, this function calls `IPAddress.fromString()` to parse the string `"192.168.0.1"`.



`convertFromJson()` must have this exact signature. Don’t change its name or parameter types; otherwise, ArduinoJson won’t find it.


---


# How to do serial communication between two boards?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/do-serial-communication-between-two-boards/

This article shows how you can send JSON documents between two boards connected by a serial link.



In this article, I’ll assume that we use two [Arduino Leonardo](https://docs.arduino.cc/hardware/leonardo/) s. I’m using this board instead of the [Arduino UNO](https://docs.arduino.cc/hardware/uno-rev3/) because it allows using `Serial` for logging and `Serial1` to communicate between the two boards.



Don’t use [`SoftwareSerial`](https://www.arduino.cc/en/Reference/SoftwareSerial) because it is **notoriously unreliable**.



## Sender program



This program sends `{"timestamp":1234,"value":687}` every 5 seconds.



```cpp
#include <ArduinoJson.h>

void setup() {
  // Initialize "debug" serial port
  // The data rate must be much higher than the "link" serial port
  Serial.begin(115200);
  while (!Serial) continue;

  // Initialize the "link" serial port
  // Use a low data rate to reduce the error ratio
  Serial1.begin(9600);
}

void loop() {
  // Values we want to transmit
  long timestamp = millis();
  int value = analogRead(1);

  // Print the values on the "debug" serial port
  Serial.print("timestamp = ");
  Serial.println(timestamp);
  Serial.print("value = ");
  Serial.println(value);
  Serial.println("---");

  // Create the JSON document
  JsonDocument doc;
  doc["timestamp"] = timestamp;
  doc["value"] = value;

  // Send the JSON document over the "link" serial port
  serializeJson(doc, Serial1);

  // Wait
  delay(5000);
}
```



## Receiver program



This program receives the JSON document and extracts the values.



```cpp
#include <ArduinoJson.h>

void setup() {
  // Initialize "debug" serial port
  // The data rate must be much higher than the "link" serial port
  Serial.begin(115200);
  while (!Serial) continue;

  // Initialize the "link" serial port
  // Use a low data rate to reduce the error ratio
  Serial1.begin(9600);
}

void loop() {
  // Check if the other Arduino is transmitting
  if (Serial1.available())
  {
    // Read the JSON document from the "link" serial port
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, Serial1);

    if (err == DeserializationError::Ok)
    {
      // Print the values
      // (we must use as<T>() to resolve the ambiguity)
      Serial.print("timestamp = ");
      Serial.println(doc["timestamp"].as<long>());
      Serial.print("value = ");
      Serial.println(doc["value"].as<int>());
    }
    else
    {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());

      // Flush all bytes in the "link" serial port buffer
      while (Serial1.available() > 0)
        Serial1.read();
    }
  }
}
```



## Things used in these programs


* [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)
  * [`operator[]`](https://arduinojson.org/v7/api/jsondocument/subscript/)
* [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/)
  * [`as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/)
* [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/)
* [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)
* [`Serial`](https://www.arduino.cc/reference/en/language/functions/communication/serial/)



## Common problems



### `IncompleteInput`



[`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) returns [`IncompleteInput`](https://arduinojson.org/v7/api/misc/deserializationerror/#incompleteinput) when the input JSON is truncated.



When applied to serial communication, this error is usually caused by:


* receiver reading too slowly and dropping bytes
* a timeout



In the first case, the sender writes bytes faster than the receiver reads them, filling up the receiver’s serial buffer until it overflows and drops incoming bytes. For example, this problem happens when the receiver is busy doing some other task when the sender is transmitting. It also occurs when the receiver logs incoming data at a slower rate; that’s why it’s crucial to get the “debug” serial running much faster than the “communication” serial.



You could solve this issue by increasing the serial buffer size; the details depend on each platform. [Arduino Leonardo](https://docs.arduino.cc/hardware/leonardo/) ‘s default is 64 and can be changed by defining the `SERIAL_RX_BUFFER_SIZE` macro. For [ESP8266](https://en.wikipedia.org/wiki/ESP8266) and [ESP32](https://en.wikipedia.org/wiki/ESP32), the default is 256 and can be changed by calling `setRxBufferSize()`.



In the second case, the sender writes bytes slower than the receiver expects, so the read times out. Usually, this problem causes an [`EmptyInput`](https://arduinojson.org/v7/api/misc/deserializationerror/#emptyinput) error, but there are a few situations where it could return [`IncompleteInput`](https://arduinojson.org/v7/api/misc/deserializationerror/#incompleteinput). In this case, you can solve the problem by [increasing the timeout](https://arduinojson.org/v7/how-to/change-the-timeout/) or by polling [`Serial::available()`](https://www.arduino.cc/reference/en/language/functions/communication/serial/available/) before calling [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/).



### `InvalidInput`



[`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) returns [`InvalidInput`](https://arduinojson.org/v7/api/misc/deserializationerror/#invalidinput) when the input JSON document is incorrect.



This problem is often due to errors in serial communication. These errors come from:


* clock skew
* electric noise
* voltage mismatch (5V vs. 3.3V)
* `SoftwareSerial`
* missing flush after error



#### Clock skew



Clock running at different speeds is a frequent problem when using different hardware on each side of the link (for example, an [AVR](https://en.wikipedia.org/wiki/AVR_microcontrollers) and an [ESP8266](https://en.wikipedia.org/wiki/ESP8266)). If the sender is too fast, the receiver will occasionally receive phantom characters; if the sender is too slow, the receiver will sometimes miss a character.



Clock accuracy is another problem of [`SoftwareSerial`](https://www.arduino.cc/en/Reference/SoftwareSerial); that’s why you should always prefer hardware implementations.



You can mitigate this problem by reducing the baud rate, but you’ll never be able to eliminate the errors completely. If you need reliable serial communication, you must detect the errors (with a checksum, for example) and resend the data when required.



#### Electrical noise



Any electric wire acts as an antenna; the longer the wire, the stronger the effect. This antenna picks up every electromagnetic field in the environment, which induces a current in the wire. On long wires, this current is strong enough to introduce errors in the transmission.



You can mitigate this problem at the hardware and software levels.



At the hardware level, you can replace the wires with a coaxial cable: the shielding will prevent the inner wire from acting as an antenna.



At the software level, you can:


1. reduce the baud rate
2. use error-correction code
3. add error detection



Reducing the communication speed always improves the error ratio but is rarely enough.



[Error-correction codes (ECC)](https://en.wikipedia.org/wiki/Error_correction_code) are a way of transmitting data with redundant information that allows the receiver to fix most of the errors. The most basic error-correction code is [Hamming(7,4)](https://en.wikipedia.org/wiki/Hamming(7,4)), which transmits 7 bits for every 4 bits of actual data. In other words, it adds 3 bits of redundancy for every 4 bits that you send. The magic with this code is that it can correct any 1-bit error in the 7 bits.



The simplest way to implement [Hamming(7,4)](https://en.wikipedia.org/wiki/Hamming(7,4)) on Arduino is to use the `HammingStream` class from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library:



```cpp
HammingStream<7, 4> eccSerial1(Serial1);
```



Now, you can use `eccSerial1` in place of the original `Serial1`; it will automatically encode and decode the information.



As [Hamming(7,4)](https://en.wikipedia.org/wiki/Hamming(7,4)) only transmits 7 bits of data, you can safely downgrade the serial link from 8 to 7 bits. You can do this by passing `SERIAL_7N1` as the second argument of [`Serial::begin()`](https://www.arduino.cc/reference/en/language/functions/communication/serial/print/). This feature is not supported by [`SoftwareSerial`](https://www.arduino.cc/en/Reference/SoftwareSerial), which is yet another reason to avoid it.



Error-correction codes are very powerful, but they’ll never eliminate errors completely. For example, [Hamming(7,4)](https://en.wikipedia.org/wiki/Hamming(7,4)) can only fix a 1-bit error, so if two or more bits are swapped, it will not fix them. To get more confidence in the integrity of the received data, the ultimate solution is to add an error detection scheme, like a checksum.



[StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) is a powerful library that deserves more attention. Please [give it a star](https://github.com/bblanchon/ArduinoStreamUtils/stargazers) to spread the word.



#### Voltage mismatch



Not all microcontrollers use the same voltage for the serial port. Some use 5V logic; others use 3.3V; the table below shows the values for the most common development boards:



| Board | Voltage |
| --- | --- |
| Adafruit Circuit Playground Express | 3.3 V |
| Adafruit Feather 32u4 Bluefruit LE | 3.3 V |
| [Adafruit HUZZAH](https://www.adafruit.com/product/2471) | 3.3 V |
| [Adafruit Metro Mini 328](https://www.adafruit.com/product/2590) | 5 V |
| Arduino Due | 3.3 V |
| [Arduino Leonardo](https://store.arduino.cc/arduino-leonardo-with-headers) | 5 V |
| Arduino M0 | 3.3 V |
| Arduino M0 Pro | 3.3 V |
| [Arduino Mega 2560 R3](https://store.arduino.cc/arduino-mega-2560-rev3) | 5 V |
| [Arduino Micro](https://store.arduino.cc/arduino-micro) | 5 V |
| Arduino MKR FOX 1200 | 3.3 V |
| Arduino MKR GSM 1400 | 3.3 V |
| Arduino MKR NB 1500 | 3.3 V |
| Arduino MKR Vidor 4000 | 3.3 V |
| Arduino MKR WAN 1300 | 3.3 V |
| Arduino MKR WAN 1310 | 3.3 V |
| Arduino MKR WiFi 1010 | 3.3 V |
| Arduino MKR ZERO | 3.3 V |
| [Arduino MKR1000](https://store.arduino.cc/arduino-mkr1000-wifi) | 3.3 V |
| [Arduino Nano](https://store.arduino.cc/arduino-nano) | 5 V |
| Arduino NANO 33 IoT | 3.3 V |
| [Arduino Nano Every](https://store.arduino.cc/arduino-nano-every) | 5 V |
| Arduino Tian | 3.3 V |
| Arduino UNO | 5 V |
| Arduino UNO WiFi | 5 V |
| [Arduino Yún](https://store.arduino.cc/arduino-yun) | 5 V |
| [Arduino Zero](https://store.arduino.cc/arduino-zero) | 3.3 V |
| diymore ESP32 CAM | 3.3 V |
| Espressif ESP32-WROVER-KIT | 3.3 V |
| HiLetgo ESP32 Camera Module Fisheye | 3.3 V |
| KeeYees ESP32-CAM | 3.3 V |
| LoLin D32 Pro | 3.3 V |
| M5Stack Fire | 3.3 V |
| MakerHawk ESP32 Camera | 3.3 V |
| Nucleo-32 | 3.3 V |
| [SparkFun RedBoard Turbo](https://www.sparkfun.com/products/14812) | 3.3 V |
| Teensy 2.0 | 5 V |
| Teensy 3.2 | 3.3 V |
| Teensy 3.5 | 3.3 V |
| Teensy 3.6 | 3.3 V |
| Teensy 4.0 | 3.3 V |
| Teensy 4.1 | 3.3 V |
| Teensy LC | 3.3 V |
| TTGO ESP32 Camera | 3.3 V |
| TTGO ESP32 WROVER | 3.3 V |
| [uPesy ESP32 Wrover DevKit](https://upesy.com/products/upesy-esp32-wrover-devkit-board) | 3.3 V |



If you need to wire two devices with different voltages, you need a [logic level converter](https://www.sparkfun.com/products/12009).



#### SoftwareSerial



The [AVR](https://en.wikipedia.org/wiki/AVR_microcontrollers) implementation of [`SoftwareSerial`](https://www.arduino.cc/en/Reference/SoftwareSerial) is **notoriously unreliable** 😱. The main problem is that it disables interrupts when sending data, which causes many issues like dropping incoming bytes on the regular [`Serial`](https://www.arduino.cc/reference/en/language/functions/communication/serial/).



You may consider the following alternative libraries, but none of them is perfect:


* [AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial)
* [SlowSoftSerial](https://github.com/MustBeArt/SlowSoftSerial)
* [NeoSoftSerial](https://github.com/SlashDevin/NeoSWSerial)



Again, the best solution is to use a board with several [UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)s, such as:



| Board | Extra ports |
| --- | --- |
| [Adafruit HUZZAH](https://www.adafruit.com/product/2471) | 1 |
| [Adafruit HUZZAH32](https://www.adafruit.com/product/3405) | 2 |
| Arduino Due | 3 |
| [Arduino Leonardo](https://store.arduino.cc/arduino-leonardo-with-headers) | 3 |
| [Arduino Mega 2560 R3](https://store.arduino.cc/arduino-mega-2560-rev3) | 3 |
| [Arduino Micro](https://store.arduino.cc/arduino-micro) | 1 |
| Arduino MKR FOX 1200 | 1 |
| Arduino MKR GSM 1400 | 1 |
| Arduino MKR NB 1500 | 1 |
| Arduino MKR Vidor 4000 | 1 |
| Arduino MKR WAN 1300 | 1 |
| Arduino MKR WAN 1310 | 1 |
| Arduino MKR WiFi 1010 | 1 |
| Arduino MKR ZERO | 1 |
| [Arduino MKR1000](https://store.arduino.cc/arduino-mkr1000-wifi) | 1 |
| Arduino UNO WiFi | 3 |
| [Arduino Yún](https://store.arduino.cc/arduino-yun) | 3 |
| [Arduino Zero](https://store.arduino.cc/arduino-zero) | 1 |
| Espressif ESP32-WROVER-KIT | 2 |
| HiLetgo ESP32 Camera Module Fisheye | 2 |
| LoLin D32 Pro | 2 |
| M5Stack Fire | 2 |
| Nucleo-32 | 2 |
| Teensy 1.0 | 1 |
| Teensy 2.0 | 1 |
| Teensy 3.0 | 3 |
| Teensy 3.1 | 3 |
| Teensy 3.2 | 3 |
| Teensy 3.5 | 6 |
| Teensy 3.6 | 6 |
| Teensy 4.0 | 7 |
| Teensy 4.1 | 8 |
| Teensy LC | 3 |
| Teensy++ 1.0 | 1 |
| Teensy++ 2.0 | 1 |
| TTGO ESP32 Camera | 2 |
| TTGO ESP32 WROVER | 2 |
| [uPesy ESP32 Wrover DevKit](https://upesy.com/products/upesy-esp32-wrover-devkit-board) | 2 |



#### Missing flush after error



[`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) may return [`InvalidInput`](https://arduinojson.org/v7/api/misc/deserializationerror/#invalidinput) because it starts reading the input mid-stream.



For example, it can happen if your program calls [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) in a loop like so:



```cpp
void loop() {
  if (Serial1.available()) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, Serial1);

    if (err) {
      Serial.println(err.c_str());
      return;
    }
}
```



The problem with this program is that, if [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) returns an error (such as [`NoMemory`](https://arduinojson.org/v7/api/misc/deserializationerror/#nomemory)), any subsequent call to [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) will return [`InvalidInput`](https://arduinojson.org/v7/api/misc/deserializationerror/#invalidinput). Indeed, [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) stops reading as soon as it encounters an error, so the remainder of the document is still in the serial buffer.



The solution is to flush the serial buffer any time an error is detected:



```cpp
void loop() {
  if (Serial1.available()) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, Serial1);

    if (err) {
      Serial.println(err.c_str());

      while (Serial1.available() > 0)
        Serial1.read();

      return;
    }
}
```



## Troubleshooting



To troubleshoot an [`InvalidInput`](https://arduinojson.org/v7/api/misc/deserializationerror/#invalidinput) error, start by displaying the JSON input. The simplest way to do that is to use the `ReadLoggingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library:



```cpp
ReadLoggingStream loggingStream(Serial1, Serial);
deserializeJson(doc, loggingStream);
```



This program will print to `Serial` every byte it receives from `Serial1`.



It’s crucial to have `Serial` running **much faster** than `Serial1`; otherwise, the program will read too slowly from `Serial1`. For example, in the program above, we used 115200 bauds for `Serial` and 9600 bauds for `Serial1`.


---


# How to use ArduinoJson with ArduinoMqttClient?


Source: https://arduinojson.org/v7/how-to/use-arduinojson-with-arduinomqttclient/

This page explains how to use ArduinoJson with [ArduinoMqttClient](https://github.com/arduino-libraries/ArduinoMqttClient/), the official MQTT client library for Arduino. It shows how to use the JSON format in MQTT messages, but you can quickly adapt the examples to use MessagePack.



[ArduinoMqttClient](https://github.com/arduino-libraries/ArduinoMqttClient/) is quite new; if you have trouble, you should consider the more mature [PubSubClient](https://arduinojson.org/v7/how-to/use-arduinojson-with-pubsubclient/) library.



## Deserializing a JSON document in MQTT message



Once your program has received an MQTT message, you can call [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) and pass the `MqttClient` instance:



```cpp
int messageSize = mqttClient.parseMessage();
if (messageSize) {
  JsonDocument doc;
  deserializeJson(doc, mqttClient);
  // use the JsonDocument as usual...
}
```



This code leverages the [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) interface implemented by `MqttClient`; that’s why you don’t need to use a temporary buffer.



You can also use the same technique if you use a callback with `MqttClient`:



```cpp
void onMqttMessage(int messageSize) {
  JsonDocument doc;
  deserializeJson(doc, mqttClient);
  // use the JsonDocument as usual...
}
```



## Serializing a JSON document into an MQTT message



Because `MqttClient` implements the [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) interface, you can pass the instance to [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/), like so:



```cpp
mqttClient.beginMessage(topic);
serializeJson(doc, mqttClient);
mqttClient.endMessage();
```



Unfortunately, this syntax limits you to a message of 256 bytes, because it goes through the internal buffer of `MqttClient`. To bypass the buffer, you must specify the size of the message to `beginMessage()`, like so:



```cpp
mqttClient.beginMessage(topic, (unsigned long)measureJson(doc));
serializeJson(doc, mqttClient);
mqttClient.endMessage();
```



The cast to `unsigned long` prevents the error “call of overloaded `beginMessage()` is ambiguous”.



## See also


* [How to use ArduinoJson with PubSubClient](https://arduinojson.org/v7/how-to/use-arduinojson-with-pubsubclient/)


---


# How to use ArduinoJson with PubSubClient?


Source: https://arduinojson.org/v7/how-to/use-arduinojson-with-pubsubclient/

This page explains how to use ArduinoJson with [PubSubClient](https://github.com/knolleary/pubsubclient/), a popular MQTT library for Arduino. It shows how to use the JSON format in MQTT messages, but you can easily adapt the examples to use MessagePack instead.



## Deserializing a JSON document in MQTT message



Once your program has subscribed to an MQTT topic, you can call [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) from the `callback` function.



```cpp
void callback(char* topic, byte* payload, unsigned int length) {
  JsonDocument doc;
  deserializeJson(doc, payload, length);
  // use the JsonDocument as usual...
}
```



### Avoiding dangling pointers



Even if you don’t use a global [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), the callback is a great place to create [dangling pointers](https://en.wikipedia.org/wiki/Dangling_pointer). Make sure you never store a pointer that you acquired in the callback:



```cpp
// BAD EXAMPLE: DON'T DO THAT!!!
const char *name = NULL;

void callback(char* topic, byte* payload, unsigned int length) {
  JsonDocument doc;
  deserializeJson(doc, payload, length);
  name = doc["name"]; // 💀 pointer is invalid as soon as function returns
}
```



Instead, make a copy of the string:



```cpp
const char name[32] = "";

void callback(char* topic, byte* payload, unsigned int length) {
  JsonDocument doc;
  deserializeJson(doc, payload, length);
  strlcpy(name, doc["name"] | "default", sizeof(name));
}
```



Of course, you can use a [`String`](https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/) if you prefer.



## Serializing a JSON document into an MQTT message



To publish a JSON document to an MQTT topic, you need to serialize it to a temporary buffer:



```cpp
char buffer[256];
serializeJson(doc, buffer);
client.publish("outTopic", buffer);
```



You can save a few CPU cycles by passing the size of the payload to `publish()`:



```cpp
char buffer[256];
size_t n = serializeJson(doc, buffer);
client.publish("outTopic", buffer, n);
```



By default, [PubSubClient](https://github.com/knolleary/pubsubclient/) limits the message size to **256 bytes** (including header); see the documentation.



### Can we avoid the temporary buffer?



It’s tempting to remove the temporary buffer to save some memory. For example, we can write:



```cpp
client.beginPublish(topic, measureJson(doc), retained);
serializeJson(doc, client);
client.endPublish();
```



However, this code is **much slower** than the one with a temporary buffer (100 to 200 times slower from our experience).



This slowness is due to the `Client` class that sends bytes one by one. To improve the speed, we need to insert a small buffer as shown in [How to improve (de)serialization speed?](https://arduinojson.org/v7/how-to/improve-speed/):



```cpp
client.beginPublish(topic, measureJson(doc), retained);
BufferingPrint bufferedClient(client, 32);
serializeJson(doc, bufferedClient);
bufferedClient.flush();
client.endPublish();
```



Notice that I used a `BufferingPrint` instead of `WriteBufferingClient` because, despite its name, the `PubSubClient` class doesn’t implement the [`Client`](https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Client.h) but the [`Print`](https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Print.h) interface. For more information on `BufferingPrint`, see the [README of the StreamUtils library](https://github.com/bblanchon/ArduinoStreamUtils).



This works, but as you can see, it’s much more complicated than the original code, so I don’t think it’s worth the effort.



## See also


* [How to use ArduinoJson with ArduinoMqttClient](https://arduinojson.org/v7/how-to/use-arduinojson-with-arduinomqttclient/)


---


# How to store a JSON document in EEPROM?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/store-a-json-document-in-eeprom/

## What is EEPROM?



EEPROM is a non-volatile memory supported on many Arduino boards. “Non-volatile” means that it keeps its content when you power off the device.



The table below summarizes the capacity of the popular IoT boards:



| Board | EEPROM capacity |
| --- | --- |
| Arduino Leonardo | 1 KB |
| Arduino Mega 2560 R3 | 4 KB |
| Arduino Micro | 1 KB |
| Arduino Nano | 1 KB |
| Arduino Nano Every | 256 B |
| Arduino UNO | 1 KB |
| Arduino UNO WiFi | 256 B |
| Arduino Yún | 1 KB |
| SparkFun RedBoard | 1 KB |
| Teensy 2.0 | 2.5 KB |
| Teensy 3.2 | 2 KB |
| Teensy 3.5 | 4 KB |
| Teensy 3.6 | 4 KB |
| Teensy 4.0 | 4 KB |
| Teensy 4.1 | 4 KB |
| Teensy LC | 128 B |
| Teensy++ 2.0 | 4 KB |



Several other platforms support an emulation for EEPROM. For example, [ESP32](https://en.wikipedia.org/wiki/ESP32) uses a file in the Flash memory to replace EEPROM.



EEPROM is a perfect location to store your application configuration, and JSON is an excellent choice for the configuration format.



The only problem with EEPROM is that it’s slow to read and extremely slow to write.



## How to use EEPROM?



The EEPROM is not mapped in the memory address space, so you must use dedicated functions. To write a byte to EEPROM, you must write:



```cpp
EEPROM.write(address, value);
```



To read a byte from EEPROM, you must write:



```cpp
value = EEPROM.read(address);
```



## How to use EEPROM with ArduinoJson?



ArduinoJson doesn’t know about [`EEPROM.read()`](https://www.arduino.cc/en/Reference/EEPROMRead) and [`EEPROM.write()`](https://www.arduino.cc/en/Reference/EEPROMWrite), so it cannot natively use EEPROM.



However, [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) and [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) support a generic interface that we can be used for virtually any kind of storage: [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/).



It’s quite easy to create a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) class that calls [`EEPROM.read()`](https://www.arduino.cc/en/Reference/EEPROMRead) and [`EEPROM.write()`](https://www.arduino.cc/en/Reference/EEPROMWrite), but you don’t need to write it yourself because the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library already provides an `EepromStream` class that does exactly this.



To write a JSON document to EEPROM, use:



```cpp
EepromStream eepromStream(address, size);
serializeJson(doc, eepromStream);
```



As you can see, we pass two arguments to the constructor of `EepromStream`: the address and the size of the region of EEPROM that we want to expose as a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/).
 If you don’t know what to use for `address`, use 0.
 If you don’t know what to use for `size`, use the value from the table above. On some platforms, you can use `EEPROM.length()`, which returns the quantity EEPROM available on the board.



Similarly, you can read a JSON document from EEPROM with:



```cpp
EepromStream eepromStream(address, size);
deserializeJson(doc, eepromStream);
```



Of course, don’t forget to include `StreamUtils.h` at the top of your source file.



[StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) is a powerful library that deserves more attention. Please [give it a star](https://github.com/bblanchon/ArduinoStreamUtils/stargazers) to spread the word.



## Notes for ESP8266 and ESP32



[ESP8266](https://en.wikipedia.org/wiki/ESP8266) and [ESP32](https://en.wikipedia.org/wiki/ESP32) don’t have a real EEPROM, but they emulate one:


* [ESP8266](https://en.wikipedia.org/wiki/ESP8266) uses a 4KB sector in the SPI Flash
* [ESP32](https://en.wikipedia.org/wiki/ESP32) uses a blob in the [Non-volatile storage (NVS)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html)



To use the EEPROM emulation on these platforms, you **must** initialize the EEPROM library:



```cpp
EEPROM.begin(512);
```



In this example, `512` is the amount of memory that you reserve for the EEPROM emulation. The documentation says you can go up to 4096 on [ESP8266](https://en.wikipedia.org/wiki/ESP8266) and 508000 on [ESP32](https://en.wikipedia.org/wiki/ESP32).



The second thing you must do is to “commit” the changes (i.e., to write the cache to the non-volatile memory). You do it like so:



```cpp
EEPROM.commit();
```



To make your code portable (i.e., work on other platforms as well), you can use `EepromStream::flush()` instead:



```cpp
eepromStream.flush();  // (calls EEPROM.commit() on ESP)
```



This function calls `EEPROM.commit()` on ESP and does nothing on other platforms.



## See also


* [Serialization tutorial](https://arduinojson.org/v7/tutorial/serialization/)
* [Deserialization tutorial](https://arduinojson.org/v7/tutorial/deserialization/)


---


# How to append a JSON object to a file?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/append-to-a-file/

**Scenario**: you have a file (on SD, SPIFFS, LittleFS, or any other filesystem) that contains a list of JSON objects, and you want to add a new object to the list.



**Question**: What’s the best way to proceed? Should you load the complete file in memory, or is there a better solution?



There are two ways to append a JSON object to a file, depending on the format of the file.



## Option 1: the file contains an array



Usually, we store a list of objects in a JSON array, like so:



```cpp
[
  {"first_name":"Stan","last_name":"Marsh"},
  {"first_name":"Kyle","last_name":"Broflovski"},
  {"first_name":"Eric","last_name":"Cartman"}
]
```



As you can see, appending an element to the array requires inserting the new object *before* the closing bracket (`]`).



This operation is not supported by the filesystem, so our only option is to load the file in memory and serialize it back, like so:



```cpp
JsonDocument doc;

// Read the file
File file = SPIFFS.open("/southpath.json", "r");
deserializeJson(doc, file);
file.close();

// Append new element
JsonObject obj = doc.add<JsonObject>();
obj["first_name"] = "Kenny";
obj["last_name"] = "McCormick";

// Write the file
file = SPIFFS.open("/southpath.json", "w");
serializeJson(doc, file);
file.close();
```



## Option 2: the file contains line-separated objects



If you want to optimize the append-to-file scenario, you must change the format of the file. Instead of wrapping the list in a JSON array, you must write the object one after the other. Usually, we insert a line break between the objects, following the pseudo-standard [JSONLines](https://jsonlines.org/) and [ndjson](http://ndjson.org/).



Here is an example:



```cpp
{"first_name":"Stan","last_name":"Marsh"}
{"first_name":"Kyle","last_name":"Broflovski"}
{"first_name":"Eric","last_name":"Cartman"}
```



As you can see, appending an element is just a matter of writing the new object at the end of the file; we don’t need to load the file in memory to do that.



```cpp
// Create the new element
JsonDocument doc;
doc["first_name"] = "Kenny";
doc["last_name"] = "McCormick";

// Append to file
File file = SPIFFS.open("/southpath.jsonl", "a");
serializeJson(doc, file);
file.println();
file.close();
```



#### How to read a JSONL file?



If you want to read this file with ArduinoJson, you simply need to call [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) repeatedly:



```cpp
File file = SPIFFS.open("/southpath.jsonl", "r");

while (true) {
  JsonDocument doc;

  DeserializationError err = deserializeJson(doc, file);
  if (err) break;

  Serial.print(doc["first_name"].as<const char*>);
  Serial.print(" ");
  Serial.println(doc["last_name"].as<const char*>);
}

file.close();
```



## See also


* [JSONLines](https://jsonlines.org/) and [ndjson](http://ndjson.org/)
* [Line-delimited JSON](https://en.wikipedia.org/wiki/JSON_streaming#Line-delimited_JSON) on Wikipedia
* “JSON streaming” in the “Advanced Techniques” chapter of [Mastering ArduinoJson](https://arduinojson.org/book/)
* [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) and [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)
* [`SPIFFS.open()`](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#open)


---


# How to use ArduinoJson with HTTPClient?


Source: https://arduinojson.org/v7/how-to/use-arduinojson-with-httpclient/

## What is `HTTPClient`?



`HTTPClient` is a class that performs HTTP requests on [ESP8266](https://en.wikipedia.org/wiki/ESP8266) and [ESP32](https://en.wikipedia.org/wiki/ESP32).



Assuming that the board is connected to the WiFi network, you can send an HTTP request like so:



```cpp
WiFiClient client;  // or WiFiClientSecure for HTTPS
HTTPClient http;

// Send request
http.begin(client, "http://arduinojson.org/example.json");
http.GET();

// Print the response
Serial.print(http.getString());

// Disconnect
http.end();
```



While you can perform HTTP requests without `HTTPClient` (cf “See also” below), this class greatly simplifies your code.



On [ESP8266](https://en.wikipedia.org/wiki/ESP8266), this class is provided by the [ESP8266HTTPClient library](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266HTTPClient), which is bundled with the [ESP8266 core for Arduino](https://github.com/esp8266/Arduino).



On [ESP32](https://en.wikipedia.org/wiki/ESP32), it’s provided by the [HTTPClient library](https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient), which is bundled with the [Arduino core for the ESP32](https://github.com/espressif/arduino-esp32).



For HTTPS, you need to use [`WiFiClientSecure`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html#bearssl-wificlientsecure-class) instead of [`WiFiClient`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-class.html).
 You must also choose the certificate validation strategy. For example, you can call [`WiFiClientSecure::setInsecure()`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html#setinsecure) to skip the validation.



## How to parse a JSON document from an HTTP response?



Unfortunately, `HTTPClient` doesn’t implement the [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) interface, so you cannot pass it directly to [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) like so:



```cpp
deserializeJson(doc, http);  // error: 'class HTTPClient' has no member named 'read'; did you mean 'end'?
```



We could pass the result of `http.getString()` directly to [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/), but it would be quite inefficient because it would copy the complete response in RAM before parsing.



We can do much better by letting ArduinoJson pull the bytes from the HTTP response. To do that, we must get `HTTPClient`’s underlying [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) by calling `http.getStream()` instead of `http.getString()`. Unfortunately, when doing so, we bypass the code that handles [chunked transfer encoding](https://en.wikipedia.org/wiki/Chunked_transfer_encoding), making the stream unusable. Let’s see two solutions to this problem.



### Solution 1: use HTTP 1.0 (recommended)



The simplest way to deal with [chunked transfer encoding](https://en.wikipedia.org/wiki/Chunked_transfer_encoding) it to disabled by downgrading to HTTP version 1.0.



```cpp
WiFiClient client;  // or WiFiClientSecure for HTTPS
HTTPClient http;

// Send request
http.useHTTP10(true);
http.begin(client, "http://arduinojson.org/example.json");
http.GET();

// Parse response
JsonDocument doc;
deserializeJson(doc, http.getStream());

// Read values
Serial.println(doc["time"].as<long>());

// Disconnect
http.end();
```



Notice the call to `http.useHTTP10(true)` before sending the request.



### Solution 2: use `ChunkDecodingStream`



If you cannot downgrade to HTTP 1.0, you can use the `ChunkDecodingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library. This class transforms a chunked-encoded stream into a regular stream. Here is how you can use it with `HTTPClient`:



```cpp
WiFiClient client;  // or WiFiClientSecure for HTTPS
HTTPClient http;

// Ask HTTPClient to collect the Transfer-Encoding header
// (by default, it discards all headers)
const char* keys[] = {"Transfer-Encoding"};
http.collectHeaders(keys, 1);

// Send request
http.begin(client, "http://arduinojson.org/example.json");
http.GET();

// Get the raw and the decoded stream
Stream& rawStream = http.getStream();
ChunkDecodingStream decodedStream(http.getStream());

// Choose the right stream depending on the Transfer-Encoding header
Stream& response =
    http.header("Transfer-Encoding") == "chunked" ? decodedStream : rawStream;

// Parse response
JsonDocument doc;
deserializeJson(doc, response);

// Read values
Serial.println(doc["time"].as<long>());

// Disconnect
http.end();
```



Notice the call to `http.collectHeaders()` before sending the request.



## How to send a JSON document in an HTTP request?



Unfortunately, `HTTPClient` doesn’t let us use the same trick as above, so we have to use a temporary buffer. Here is how we can do so with a [`String`](https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/):



```cpp
// Prepare JSON document
JsonDocument doc;
doc["hello"] = "world";

// Serialize JSON document
String json;
serializeJson(doc, json);

WiFiClient client;  // or WiFiClientSecure for HTTPS
HTTPClient http;

// Send request
http.begin(client, "http://httpbin.org/post");
http.POST(json);

// Read response
Serial.print(http.getString());

// Disconnect
http.end();
```



If you run this program, you’ll see that it prints information on the HTTP requests. That’s the response returned by [httpbin.org](http://httpbin.org/); it’s very handy to debug your programs.



You can significantly improve this code’s performance by calling [`String::reserve()`](https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/reserve/) before [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/).
 [Here are some other tips for using the `String` class efficiently](https://cpp4arduino.com/2018/11/21/eight-tips-to-use-the-string-class-efficiently.html).



If you want to send the payload as a stream, you need to use `WiFiClient` directly, as shown in the [Serialization Tutorial](https://arduinojson.org/v7/tutorial/serialization/) (around page 127).



## Why is the `JsonDocument` empty?



The most likely explanation is that the server returned a [redirection code](https://developer.mozilla.org/en-US/docs/Web/HTTP/Redirections), like [301 Moved Permanently](https://en.wikipedia.org/wiki/HTTP_301) or [302 Found](https://en.wikipedia.org/wiki/HTTP_302).



```cpp
http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
```



This feature was added quite recently on [ESP32](https://en.wikipedia.org/wiki/ESP32), so make sure you are up-to-date.



## BONUS: How to see the document when I use a `Stream`?



As we saw, it’s more efficient to pass the [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) because it saves a large amount of memory. However, we cannot directly print the content of the [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) to see what was sent to ArduinoJson.



To see the content that is received from the HTTP response, we can use the `ReadLoggingStream` from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library.



Replace:



```cpp
deserializeJson(doc, http.getStream());
```



with:



```cpp
ReadLoggingStream loggingStream(http.getStream(), Serial);
deserializeJson(doc, loggingStream);
```



`ReadLoggingStream` will forward everything to [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) and will print the content to the serial port.



[StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) is a powerful library that deserves more attention. Please [give it a star](https://github.com/bblanchon/ArduinoStreamUtils/stargazers) to spread the word.



## See also


* The GitHub example in chapter 3 of [Mastering ArduinoJson](https://arduinojson.org/book/) uses `HTTPClient` to interact with GitHub’s API.
 This chapter is available for free, see [Deserialization Tutorial](https://arduinojson.org/v7/tutorial/deserialization/).
* The Reddit case study in chapter 8 of [Mastering ArduinoJson](https://arduinojson.org/book/) shows how to perform HTTP requests **without `HTTPClient`**.


---


# How to use external RAM on ESP32?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/use-external-ram-on-esp32/

## What is the external RAM?



The [ESP32](https://en.wikipedia.org/wiki/ESP32) chip contains 520KB of RAM. While it’s sufficient for most projects, others may need more memory. To increase the capacity of the microcontroller, the manufacturer can add a memory chip to the board. This external RAM chip is connected to the [ESP32](https://en.wikipedia.org/wiki/ESP32) via the [SPI bus](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface).



For example, the following boards embed such a chip:



| Board | External RAM |
| --- | --- |
| diymore ESP32 CAM | 4 MB |
| Espressif ESP32-WROVER-KIT | 4 MB |
| HiLetgo ESP32 Camera Module Fisheye | 8 MB |
| KeeYees ESP32-CAM | 4 MB |
| LoLin D32 Pro | 8 MB |
| M5Stack Fire | 4 MB |
| MakerHawk ESP32 Camera | 8 MB |
| TTGO ESP32 Camera | 8 MB |
| TTGO ESP32 WROVER | 8 MB |
| [uPesy ESP32 Wrover DevKit](https://upesy.com/products/upesy-esp32-wrover-devkit-board) | 4 MB |



In theory, any SPI memory chip could be used, but in practice, it’s always the same: the ESP-PSRAM32. Because this chip uses a technology known as [Pseudostatic RAM (PSRAM)](https://en.wikipedia.org/wiki/PSRAM), we often use the name “PSRAM” when we should really say “External SPI RAM.”



Using the extended memory requires extra work from the programmer: you need to call dedicated allocation functions. Instead of the good old [`malloc()`](https://en.cppreference.com/w/c/memory/malloc), you must call [`heap_caps_malloc(MALLOC_CAP_SPIRAM)`](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/mem_alloc.html).



## How to use the PSRAM with ArduinoJson?



As we just saw, to use the PSRAM, a program must use dedicated allocation functions, which means we cannot use [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) as before. Instead, we must provide a custom allocator that uses `heap_caps_malloc()`.



To do this, you must create a new class that implements the `ArduinoJson::Allocator` interface, like so:



```cpp
struct SpiRamAllocator : ArduinoJson::Allocator {
  void* allocate(size_t size) override {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) override {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) override {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};
```



This snippets defines `SpiRamAllocator` which you can use like so:



```cpp
SpiRamAllocator allocator;
JsonDocument doc(&allocator);
deserializeJson(doc, input);
```



You don’t need to do anything else.



You probably don’t need `SpiRamAllocator` if you updated the Arduino core; see below.



## Alternative solution



Alternatively, you can ask the [ESP32](https://en.wikipedia.org/wiki/ESP32) to [include external RAM into the classic `malloc()` function](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/external-ram.html#provide-external-ram-via-malloc) so that a program can use both RAMs without modification. In this mode, [`malloc()`](https://en.cppreference.com/w/c/memory/malloc) returns memory blocks from either the internal or the external RAM, which means you don’t need a custom allocator.



To use this mode, you must configure [CONFIG_SPIRAM_USE](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-spiram-use) to `SPIRAM_USE_MALLOC`.



### Arduino core for ESP32 version 1



This setting can be set in the following file:


* `%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\1.0.6\tools\sdk` (on Windows)
* `~/.arduino15/packages/esp32/hardware/esp32/1.0.6/tools/sdk/sdkconfig` (on Linux and macOS)



In the file `sdkconfig`, you’ll find a block like this:



```cpp
#
# SPI RAM config
#
CONFIG_SPIRAM_BOOT_INIT=
CONFIG_SPIRAM_USE_MEMMAP=
CONFIG_SPIRAM_USE_CAPS_ALLOC=y
CONFIG_SPIRAM_USE_MALLOC=
CONFIG_SPIRAM_TYPE_AUTO=y
CONFIG_SPIRAM_TYPE_ESPPSRAM32=
CONFIG_SPIRAM_TYPE_ESPPSRAM64=
CONFIG_SPIRAM_SIZE=-1
CONFIG_SPIRAM_SPEED_40M=y
CONFIG_SPIRAM_CACHE_WORKAROUND=y
CONFIG_SPIRAM_BANKSWITCH_ENABLE=y
CONFIG_SPIRAM_BANKSWITCH_RESERVE=8
CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST=
CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY=
```



You can set `CONFIG_SPIRAM_USE_MALLOC=y` to include PSRAM in [`malloc()`](https://en.cppreference.com/w/c/memory/malloc)’s scope.



The problem is this file is volatile: it gets replaced when you update the Arduino core.



### Arduino core for ESP32 version 2



Version 2 has separate configurations per ESP generation:


* On Windows:


  1. `%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\2.0.2\tools\sdk\esp32` (for ESP32)
  2. `%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\2.0.2\tools\sdk\esp32s2` (for ESP32-S2)
  3. `%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\2.0.2\tools\sdk\esp32c3` (for ESP32-C3)
* On Linux and macOS:


  1. `~/.arduino15/packages/esp32/hardware/esp32/2.0.2/tools/sdk/esp32` (for ESP32)
  2. `~/.arduino15/packages/esp32/hardware/esp32/2.0.2/tools/sdk/esp32s2` (for ESP32-S2)
  3. `~/.arduino15/packages/esp32/hardware/esp32/2.0.2/tools/sdk/esp32c3` (for ESP32-C3)



In the file `sdkconfig`, you’ll find a block like this:



```cpp
#
# SPI RAM config
#
CONFIG_SPIRAM_TYPE_AUTO=y
# CONFIG_SPIRAM_TYPE_ESPPSRAM16 is not set
# CONFIG_SPIRAM_TYPE_ESPPSRAM32 is not set
# CONFIG_SPIRAM_TYPE_ESPPSRAM64 is not set
CONFIG_SPIRAM_SIZE=-1
CONFIG_SPIRAM_SPEED_40M=y
CONFIG_SPIRAM=y
# CONFIG_SPIRAM_BOOT_INIT is not set
# CONFIG_SPIRAM_USE_MEMMAP is not set
# CONFIG_SPIRAM_USE_CAPS_ALLOC is not set
CONFIG_SPIRAM_USE_MALLOC=y
CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL=4096
CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP=y
CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL=0
# CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY is not set
# CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY is not set
CONFIG_SPIRAM_CACHE_WORKAROUND=y
```



As you can see, `CONFIG_SPIRAM_USE_MALLOC` is already set to `y`, so [`malloc()`](https://en.cppreference.com/w/c/memory/malloc) already returns blocks from the PSRAM. In other words, **you don’t have to change any setting** when using version 2 of the core.



## See also


* [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)
* [How to use PSRAM on uPesy](https://www.upesy.com/blogs/tutorials/get-more-ram-on-esp32-with-psram)


---


# How to use string_view on ESP32?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/use-string-view-on-esp32/

[`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view) represents a constant string that doesn’t have to be null-terminated. Unlike [`std::string`](https://en.cppreference.com/w/cpp/string/basic_string), it doesn’t store a copy of the original string but only its location. This class is typically implemented with two pointers or with a pointer and an integer.



[`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view) was officially introduced in the C++ standard library with C++17. The Arduino core from [ESP32](https://en.wikipedia.org/wiki/ESP32) comes with an old version of GCC that doesn’t support C+17, so we cannot use [`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view) on [ESP32](https://en.wikipedia.org/wiki/ESP32). Or can we?



Even if [`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view) only came out officially with C++17, it was in the boxes for a long time. GCC 5.2, the compiler used for [ESP32](https://en.wikipedia.org/wiki/ESP32), comes with an experimental version [`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view) that we can use with ArduinoJson.



> > #### Disclaimer



> The workaround presented here is very hackish and goes against recommended practices. It is only acceptable because it is a temporary solution until the compiler gets upgraded.



> This article was written for version 1.0.6 of the core; please adapt to the version installe on your machine.



To use use [`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view) on [ESP32](https://en.wikipedia.org/wiki/ESP32), we must:


1. recreate the [`<string_view>`](https://en.cppreference.com/w/cpp/header/string_view) header from [`<experimental/string_view>`](https://en.cppreference.com/w/cpp/header/experimental/string_view)
2. import [`std::experimental::string_view`](https://en.cppreference.com/w/cpp/experimental/basic_string_view) into the `std` namespace
3. set [`ARDUINOJSON_ENABLE_STRING_VIEW`](https://arduinojson.org/v7/config/enable_string_view/) to `1`
4. change the flag `-std=gnu++11` to `-std=gnu++1y`



### Steps 1 and 2



For the two first steps, locate the standard library folder. On Windows, it’s currently:



```cpp
%LOCALAPPDATA%\Arduino15\packages\esp32\tools\xtensa-esp32-elf-gcc\1.22.0-97-gc752ad5-5.2.0\xtensa-esp32-elf\include\c++\5.2.0
```



In this folder, create a file named `string_view` (without extension) with the following content:



```cpp
#include "experimental/string_view"
namespace std { using std::experimental::string_view; }
```



### Step 3



For the third step, you simply need to add the following block at the beginning of your program:



```cpp
#define ARDUINOJSON_ENABLE_STRING_VIEW 1
#include <ArduinoJson.h>
```



### Step 4



Step 4 requires that you change the compiler flags in `platform.txt`. On Windows, this file is currently located here:



```cpp
%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\1.0.6\platform.txt
```



In this file, locate the line that starts with `compiler.cpp.flags=` and replace `-std=gnu++11` with `-std=gnu++17` (it’ literally a one-character change). You don’t need to restart the Arduino IDE after modifying this file.



### Test



You should now be able to use [`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view). Use the following program to test you setup:



```cpp
#define ARDUINOJSON_ENABLE_STRING_VIEW 1
#include <ArduinoJson.h>

void setup() {
  Serial.begin(115200);

  JsonDocument doc;
  doc[std::string_view("hello!", 5)] = std::string_view("world!", 5);

  serializeJson(doc, Serial);  // prints {"hello":"world"}
}

void loop() {
}
```


---


# How to change the timeout when reading from a Stream?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/change-the-timeout/

## Fixing error `IncompleteInput`



When you read from a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/), [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) and [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/) can return [`IncompleteInput`](https://arduinojson.org/v7/api/misc/deserializationerror/) if a timeout occurs.



Indeed, the stream waits for the data coming from the other peer, but if there is no data for an extended period, the stream returns a timeout error. The timeout is set to 1 second by default, but you can change this value in your program.



To configure the timeout, you must call [`Stream::setTimeout()`](https://www.arduino.cc/reference/en/language/functions/communication/stream/streamsettimeout/) before calling [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) or [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/), like that:



```cpp
// set timeout to 10 seconds
Serial.setTimeout(10000);

// read JSON document from Serial
deserializeJson(doc, Serial);
```



The example above uses the serial port, but you can use the same technique for any class derived from [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/), like [`EthernetClient`](https://www.arduino.cc/en/Reference/EthernetClient), [`WifiClient`](https://www.arduino.cc/en/Reference/WiFiClient), [`SoftwareSerial`](https://www.arduino.cc/en/Reference/SoftwareSerial).



## Still not working?



If increasing the timeout doesn’t solve your problem, you can diagnose the problem with the `ReadLoggingStream` decorator from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library. This class adds the logging functionality to an existing stream class, so you can see what is read from the stream.



Here is how to use `ReadLoggingStream`:



```cpp
// set timeout to 10 seconds
wifiClient.setTimeout(10000);

// decorate the WifiClient so it logs its content to Serial
ReadLoggingStream wifiClientWithLog(wifiClient, Serial);

// read JSON document from the WifiClient
deserializeJson(doc, wifiClientWithLog);
```



This way, you’ll see where the reading stops and why ArduinoJson returns `IncompleteInput`.



[StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) is a powerful library that deserves more attention. Please [give it a star](https://github.com/bblanchon/ArduinoStreamUtils/stargazers) to spread the word.



## See also


* [Deserialization tutorial](https://arduinojson.org/v7/tutorial/deserialization/)
* [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)
* [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/)
* [`DeserializationError`](https://arduinojson.org/v7/api/misc/deserializationerror/)


---


# How to deserialize a very large document?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/deserialize-a-very-large-document/

By design, ArduinoJson stores the complete JSON document in RAM. Therefore, it cannot deserialize a document larger than your microcontroller’s capacity. The [ArduinoJson Assistant](https://arduinojson.org/v7/assistant/) will warn you if you try to deserialize a document that is too big.



Some [ESP32](https://en.wikipedia.org/wiki/ESP32)s come with a large [external PSRAM that you can use with ArduinoJson](https://arduinojson.org/v7/how-to/use-external-ram-on-esp32/).



ArduinoJson supports two techniques to deserialize very large inputs: “filtering” and “deserialization in chunks.” These techniques allow you to read a document that would otherwise not fit in the RAM of your microcontroller.



## Filtering



[`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) supports a filtering feature that reduces memory consumption by ignoring irrelevant fields from the input document. It allows you to keep only the fields that matter in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



To use this feature, you must create an ancillary [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) that will serve as a filter. This document must contain the value `true` as a placeholder for each field you want to keep; every other field will be discarded. For arrays, only create one element in the filter document; it will serve as the filter for every element of the original document.



For example, suppose your input looks like this:



```cpp
{
  "list": [
    {"temperature":21.2,"humidity":68.9,"pressure":1003},
    {"temperature":19.7,"humidity":62.1,"pressure":1007},
    {"temperature":18.6,"humidity":59.8,"pressure":1009}
   ]
}
```



If you only want to keep the temperature field, you must create the following filter:



```cpp
{
  "list": [
    {"temperature":true}
   ]
}
```



Let’s put this example into code. Here is how we can create the filter document:



```cpp
JsonDocument filter;
filter["list"][0]["temperature"] = true;
```



Once the filter document is ready, you must wrap it with `DeserializationOption::Filter`, and pass it to [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/):



```cpp
deserializeJson(doc, input, DeserializationOption::Filter(filter));
```



After executing this line, `doc` will contains the following document:



```cpp
{
  "list": [
    {"temperature":21.2},
    {"temperature":19.7},
    {"temperature":18.6}
   ]
}
```



In this case, filtering reduced the memory consumption in half.



The **OpenWeatherMap** case study in [Mastering ArduinoJson](https://arduinojson.org/book/) shows how to use this technique in a real project.



## Deserialization in chunks



Instead of parsing the whole JSON document in one shot, you can parse only a part of it and repeat the operation.



For example, suppose you are interested in the “characters” array from the following JSON document:



```cpp
{
   "characters": [
        {"name":"Homer Simpson"},
        {"name":"Marge Simpson"},
        {"name":"Bart Simpson"},
        {"name":"Lisa Simpson"},
        // ...
    ]
}
```



If you try to deserialize the JSON document, it will probably not fit in the RAM of the microcontroller. However, you could easily deserialize each JSON object (i.e., each “character”), one by one.



One cool feature of ArduinoJson is that, when it parses an object from a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/), it stops reading as soon as it sees the closing brace (`}`). The same is true with arrays: it stops reading as soon as it sees the closing bracket (`]`).



This feature allows you to parse streams in chunks: you just need to call [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) in a loop. Of course, you must skip the commas (`,`) between the objects.



Here is how this technique works:


1. Jump to the beginning of the array.
2. Call [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)
3. Read the next character; if it’s a comma, go to 2.



For step 1, you can use [`Stream::find()`](https://www.arduino.cc/reference/en/language/functions/communication/stream/streamfind/); for step 2, you can use [`Stream::findUntil()`](https://www.arduino.cc/reference/en/language/functions/communication/stream/streamfinduntil/)



Here is how you can implement this technique:



```cpp
input.find("\"characters\":[");
do {
    deserializeJson(doc, input);
    // ...
} while (input.findUntil(",","]"));
```



The **Reddit** case study in [Mastering ArduinoJson](https://arduinojson.org/book/) shows how to use this technique in a real project.



This technique only works for arrays; you cannot deserialize a large object in chunks.



## Combining both techniques



You can also combine both techniques to deserialize in chunks *and* filter the content of each chunk.



The code is similar to the one above, except that you pass a filter document to [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/):



```cpp
JsonDocument filter;
filter["name"] = true;

input.find("\"characters\":[");
do {
    deserializeJson(doc, input, DeserializationOption::Filter(filter));
    // ...
} while (input.findUntil(",","]"));
```



## See also


* [json-streaming-parser](https://github.com/squix78/json-streaming-parser) is another JSON library for Arduino but with a very different design. Instead of loading the document in memory, it invokes a callback for each input’s token. This can be a solution if none of the above applies but at the price of a convoluted code.


---


# How to debug data coming from a stream?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/debug-data-coming-from-a-stream/

ArduinoJson can read data coming from either a buffer or a stream. I encourage you to use a stream because it consumes less memory and often leads to shorter programs; unfortunately, it’s harder to debug.



On this page, we’ll see how to debug the input coming from a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/), like [`EthernetClient`](https://www.arduino.cc/en/Reference/EthernetClient), [`WifiClient`](https://www.arduino.cc/en/Reference/WiFiClient), [`Serial`](https://www.arduino.cc/reference/en/language/functions/communication/serial/), etc.



## How to print the content of a stream



When you use a buffer, you can easily debug the input JSON document by writing it to the serial port. However, when you use a stream, [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) consumes the input and doesn’t allow you to get a sneak peek at it. At least, not directly, we’ll need the help of another library.



We can print the content of the stream being consumed by using the `ReadLoggingStream` class from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library. This class is a [decorator](https://en.wikipedia.org/wiki/Decorator_pattern) that adds the logging ability to any stream. To construct a `ReadLoggingStream`, you must pass the input stream ([`WifiClient`](https://www.arduino.cc/en/Reference/WiFiClient), [`EthernetClient`](https://www.arduino.cc/en/Reference/EthernetClient), `File`…) as the first argument, and the destination stream ([`Serial`](https://www.arduino.cc/reference/en/language/functions/communication/serial/), most likely) as the second argument. Then, you can use the new instance in place of the original stream.



The following schema, taken from the documentation of [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils), depicts the way data travels in `ReadLoggingStream`:



In Arduino, a [`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) is bidirectional: you can both write to it and read from it.


* The blue line (at the top) shows how data is written to the stream; it passes through the decorator unaltered.
* The orange line shows how data is read from the stream; it passes through the decorator, but it’s also copied to the logging stream (`Serial` in most cases).



In other word, every byte that ArduinoJson pulls from the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) gets printed to the serial port.



## Example



For example, let’s say you’re reading a JSON document from a WiFi connection:



```cpp
deserializeJson(doc, wifiClient);
```



If you want to log the content of the JSON document to the serial port, you must replace this line with:



```cpp
ReadLoggingStream loggingStream(wifiClient, Serial);
deserializeJson(doc, loggingStream);
```



Of course, don’t forget to install the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) and to add `#include <StreamUtils.h>` at the top of the file.



[StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) is a powerful library that deserves more attention. Please [give it a star](https://github.com/bblanchon/ArduinoStreamUtils/stargazers) to spread the word.



## See also


* [README of StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils)
* [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)
* [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/)


---


# How to validate a JSON document?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/validate-json/

ArduinoJson never intended to be a JSON validation library, so it doesn’t provide any built-in solution. However, you can use the following function to test whether a JSON string *seems* valid.



This solution doesn’t detect all errors but should be good enough for most projects.



```cpp
// Returns true if input points to a valid JSON string
bool validateJson(const char* input) {
  JsonDocument doc, filter;
  return deserializeJson(doc, input, DeserializationOption::Filter(filter)) == DeserializationError::Ok;
}
```



As you can see, it uses a null [filter](https://arduinojson.org/news/2020/03/22/version-6-15-0/) to skip all the values in the input. Without this filter [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) would return [`NoMemory`](https://arduinojson.org/v7/api/misc/deserializationerror/#nomemory). Because [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) is skipping the values, it will oversee many errors in the input.



Thank you to [Jeroen Döll](https://github.com/Jeroen88) for finding this technique.


---


# How to configure the serialization of floats?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/configure-the-serialization-of-floats/

## How does ArduinoJson serialize floats?



When you call [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/), ArduinoJson converts each floating point value into a string with a variable number of decimal places. This behavior differs from [`Serial::print()`](https://www.arduino.cc/reference/en/language/functions/communication/serial/print/) which uses a fixed number of decimal places.



ArduinoJson uses as few digits as possible: it doesn’t add trailing zeros. It uses up to 9 decimal places; this is a limitation of the [lightweight float-to-string algorithm](https://blog.benoitblanchon.fr/lightweight-float-to-string/).



When the value is very large (above 1e7), ArduinoJson switches to the scientific notation and adds an exponent to the value. It uses the same notation if the value is very small (below 1e-5).



Here are some examples:



| Original value | JSON output | Explanation |
| --- | --- | --- |
| `3.14` | `"3.14"` |  |
| `3.14159265359` | `"3.141592654"` | Only 9 decimal places |
| `1.0` | `"1"` | No trailing zeros |
| `0.9999999999` | `"1"` | Rounding |
| `10000000.0` | `"1e7"` | Exponentiation |
| `0.00001` | `"1e-5"` | Exponentiation |



## How to reduce the number of decimal places?



If you find that there are too many digits, you can round the floating point value. Use the function below to have a maximum of two decimal places.



```cpp
// rounds a number to 2 decimal places
// example: round(3.14159) -> 3.14
double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}
```



## How to add trailing zeros?



If you need to get the trailing zeros, you need to bypass ArduinoJson’s conversion algorithm with [`serialized()`](https://arduinojson.org/v7/api/misc/serialized/).



Fortunately, you don’t have to implement your own algorithm; you can reuse the one implemented in Arduino’s [`String`](https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/), which uses a fixed number of decimal places:



```cpp
doc["value"] = serialized(String(1.0,6)); // 1.000000
```



## How to disable exponentiation?



It’s possible to tweak the conversion by changing the following settings:


* [`ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD`](https://arduinojson.org/v7/config/positive_exponentiation_threshold/) is the limit for large values. Above this threshold, values use scientific notation. The default is `1e7`, and the maximum you can set is `1e9` (this is also a limitation of the [lightweight float-to-string algorithm](https://blog.benoitblanchon.fr/lightweight-float-to-string/)).
* [`ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD`](https://arduinojson.org/v7/config/negative_exponentiation_threshold/) is limit for small values. Below this threshold, values use scientific notation. The default is `1e-5`.



These two settings give some margin but don’t allow disabling the scientific notation entirely. If you want to disable it, you need to bypass ArduinoJson’s conversion algorithm with [`serialized()`](https://arduinojson.org/v7/api/misc/serialized/).



```cpp
doc["value"] = serialized(String(1e9)); // 1000000000
```


---


# How to compute the hash of a JSON document?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/compute-hash/

You don’t need to serialize the document to a temporary buffer to compute its hash (e.g., [CRC](https://en.wikipedia.org/wiki/Cyclic_redundancy_check) or [MD5](https://en.wikipedia.org/wiki/MD5)).
 Instead, you can write a [custom writer](https://arduinojson.org/news/2019/11/01/version-6-13-0/#custom-writer) that will do that on the fly.



Here is a writer that uses the [FastCRC](https://github.com/FrankBoesing/FastCRC) library to compute the [CRC32](https://en.wikipedia.org/wiki/Cyclic_redundancy_check#CRC-32_algorithm) of a JSON or MessagePack document:



```cpp
#include <FastCRC.h>  // https://github.com/FrankBoesing/FastCRC

class CrcWriter {
public:
  CrcWriter() {
    _hash = _hasher.crc32(NULL, 0);
  }

  size_t write(uint8_t c) {
    _hash = _hasher.crc32_upd(&c, 1);
    return 1;
  }

  size_t write(const uint8_t *buffer, size_t length) {
    _hash = _hasher.crc32_upd(buffer, length);
    return length;
  }

  uint32_t hash() const {
    return _hash;
  }

private:
  FastCRC32 _hasher;
  uint32_t _hash;
};
```



Use this class like so:



```cpp
CrcWriter writer;
serializeJson(doc, writer);
Serial.println(writer.hash());
```


---


# How to improve (de)serialization speed?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/improve-speed/

ArduinoJson is not slow by itself, but it looks slow when used with **unbuffered streams** because it often reads and writes characters one at a time.



The following classes are impacted (not exhaustive): [`EthernetClient`](https://www.arduino.cc/en/Reference/EthernetClient), [`WifiClient`](https://www.arduino.cc/en/Reference/WiFiClient), [`File`](https://www.arduino.cc/en/Reference/SD), [PubSubClient](https://github.com/knolleary/pubsubclient/).



## Speed up serialization



As an example, we’ll take the [`WiFiClient`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-class.html) class from the [ESP8266 core](https://github.com/esp8266/Arduino). Because ArduinoJson writes bytes one by one, [`WiFiClient`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-class.html) spends a lot of time sending small packets. To speed up your program, you need to insert a buffer between [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) and [`WiFiClient`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-class.html). You can do that using the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library.



Suppose your program is currently like that:



```cpp
serializeJson(doc, wifiClient);
```



To add buffering, replace the line above with the followings:



```cpp
WriteBufferingStream bufferedWifiClient(wifiClient, 64);
serializeJson(doc, bufferedWifiClient);
bufferedWifiClient.flush();
```



The first line creates a new stream `bufferedWifiClient` that implements buffering on top of the original [`WiFiClient`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-class.html) (this is the [Decorator pattern](https://en.wikipedia.org/wiki/Decorator_pattern)).
 The second line writes the JSON document to the [`WiFiClient`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-class.html) through the buffer.
 The last line flushes the buffer to make sure we send the end of the document.



## Speed up deserialization



As an example, we’ll take the [`File`](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#file-object) class from the [ESP8266 core](https://github.com/esp8266/Arduino). Because ArduinoJson reads bytes one by one, [`File`](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#file-object) spends a lot of time transmitting small packets over the SPI bus. To speed up your program, you must insert a buffer between [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) and [`File`](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#file-object). You can do that using the [StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) library.



Suppose your program is currently like so:



```cpp
deserializeJson(doc, file);
```



To add buffering, replace the line above with the followings:



```cpp
ReadBufferingStream bufferedFile(file, 64);
deserializeJson(doc, bufferedFile);
```



The first line creates a new stream `bufferedFile` that implements buffering on top of the original [`File`](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#file-object) (this is the [Decorator pattern](https://en.wikipedia.org/wiki/Decorator_pattern)).
 The second line reads the JSON document from the [`File`](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#file-object) through the buffer.



## See also



Check out the [README file of StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils/blob/master/README.md) to see the other things you can do with this library.



[StreamUtils](https://github.com/bblanchon/ArduinoStreamUtils) is a powerful library that deserves more attention. Please [give it a star](https://github.com/bblanchon/ArduinoStreamUtils/stargazers) to spread the word.


---


# How to reduce memory usage?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/reduce-memory-usage/

One of the biggest challenges of microcontrollers is the small quantity of RAM they offer. On this page, we’ll see how to write Arduino programs that consume less memory. Of course, the focus is on JSON serialization and deserialization, but some tips are applicable even when you don’t use ArduinoJson.



Some [ESP32](https://en.wikipedia.org/wiki/ESP32)s come with a large [external PSRAM that you can use with ArduinoJson](https://arduinojson.org/v7/how-to/use-external-ram-on-esp32/).



## Tip 1: Pass `Stream` directly to the deserializer



A stream ([`Stream`](https://www.arduino.cc/reference/en/language/functions/communication/stream/) or [`std::istream`](https://en.cppreference.com/w/cpp/io/basic_istream)) is a volatile bytes source, so its content needs to be copied into RAM. The best way to deal with streams is to let ArduinoJson copy the content because it ignores everything it can: punctuation, spaces, comments, and duplicated strings.



To do that, simply pass the stream to [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) or [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/).



> > ##### Good: pass the Stream directly



> ```cpp
File file = SD.open(filename);
deserializeJson(doc, file);
> ```



> > ##### Bad: pass a copy of the input



> ```cpp
char buffer[256];
File file = SD.open(filename);
file.read(buffer, 256);
JsonDocument doc;
deserializeJson(doc, buffer);
> ```



> > ##### Worse: copy the input on the heap



> ```cpp
File file = SD.open(filename);
size_t fileSize = file.size();
std::unique_ptr<char[]> buffer(new char[fileSize]);
file.read(buffer.get(), fileSize);
JsonDocument doc;
deserializeJson(doc, buffer.get(), fileSize);
> ```



> > ##### Even worse: manage heap memory manually



> ```cpp
File file = SD.open(filename);
size_t fileSize = file.size();
char* buffer = (char*)malloc(fileSize);
file.read(buffer, fileSize);
JsonDocument doc;
deserializeJson(doc, buffer, fileSize);
free(buffer);
> ```



##### See also:


* [JsonHttpClient.ino](https://arduinojson.org/v7/example/http-client/)
* [JsonConfigFile.ino](https://arduinojson.org/v7/example/config/)
* [Deserialization Tutorial](https://arduinojson.org/v7/tutorial/deserialization/)



## Tip 2: Move longs strings literals to Flash memory



Flash (or [`PROGMEM`](https://www.arduino.cc/reference/en/language/variables/utilities/progmem/)) strings are excellent for reducing the RAM usage of a program: contrary to regular string literals, they only use RAM when the program pulls them from the Flash.



**This advice is only relevant to [Harvard architectures](https://en.wikipedia.org/wiki/Harvard_architecture)** (mainly [AVR](https://en.wikipedia.org/wiki/AVR_microcontrollers) and [ESP8266](https://en.wikipedia.org/wiki/ESP8266)).
 It does not apply to [von Neumann architectures](https://en.wikipedia.org/wiki/Von_Neumann_architecture) (such as [ESP32](https://en.wikipedia.org/wiki/ESP32), megaAVR, and ARM).
 See the first chapter of [Mastering ArduinoJson](https://arduinojson.org/book/) for an explanation.



> > ##### Good: long strings literals in Flash



> ```cpp
doc[F("description")] = F("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
> ```



> > ##### Bad: long strings literals in RAM



> ```cpp
doc["description"] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
> ```



> > ##### Good: log strings in Flash



> ```cpp
Serial.print(F("deserializeJson() failed with error code "));
Serial.println(err.f_str());
> ```



> > ##### Bad: log strings in RAM



> ```cpp
Serial.print("deserializeJson() failed with error code ");
Serial.println(err.c_str());
> ```



The compiler doesn’t deduplicate strings created by the `F()` macro. Don’t repeat the same string multiple times; instead, save the pointer in a variable of type `const __FlashStringHelper*`.



## Tip 3: Filter input



You can tell [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) to filter the input to keep only the relevant parts.



This technique only makes sense if the input contains a lot of information that you are not interested in. A typical example is a web service like [OpenWeatherMap](https://openweathermap.org/api), which returns many fields, but only a few are relevant to your project.



For the implementation details, see:


* [How to deserialize a very large document?](https://arduinojson.org/v7/how-to/deserialize-a-very-large-document/)
* The “Advanced Techniques” chapter in [Mastering ArduinoJson](https://arduinojson.org/book/)
* The “OpenWeatherMap” case study in [Mastering ArduinoJson](https://arduinojson.org/book/)



## Tip 4: Deserialize in chunks



One neat feature of ArduinoJson is that, when it parses an object from a Stream, it stops reading when it encounters the closing `}`, and the same is true for arrays. Using this feature, you don’t have to deserialize the whole JSON document at once. Instead, you can parse only a part of it and repeat the operation.



This technique works great when your input contains an array of nested objects. For example, if you want to parse the huge response of a 10-day forecast of Weather Underground, you can skip the beginning until you see `"forecastday": [` in the stream (use [`Stream::find()`](https://www.arduino.cc/reference/en/language/functions/communication/stream/streamfind/)), and then parse the objects for each day one after the other.



For the implementation details, see:


* [How to deserialize a very large document?](https://arduinojson.org/v7/how-to/deserialize-a-very-large-document/)
* The “Advanced Techniques” chapter in [Mastering ArduinoJson](https://arduinojson.org/book/)
* The “Reddit” case study in [Mastering ArduinoJson](https://arduinojson.org/book/)



## Tip 5: Avoid global variables



Global variables are harmful on many levels; we’ll only look at memory usage.



The problem with global variables is that, by definition, they live during the whole execution of the program. In other words, they always consume RAM, whether the program uses them or not. Contrast that with local variables with a short lifespan; they only consume memory when the program needs them.



My advice is: reduce the number and the size of global variables to the strict minimum.



## Tip 6: Avoid duplication of `String`



Remember that the `String` class always makes a copy of the string passed to the constructor.



For example, the following line makes a copy of “hello world”:



```cpp
String s = "hello world"; // one copy
```



Yes, it means that “hello world” is present twice in RAM: in the global section and in the heap. If you just need to give a name to this variable, it’s better to use the actual type of the string literal, which is `const char*`:



```cpp
const char* s = "hello world"; // no copy
```



Another common mistake is to declare a function that takes a parameter of type `String` by value:



```cpp
void f(String s) {
  // ...
}
```



Because the function parameter is a value (not a pointer, nor a reference), each function invocation creates a new `String`, thereby creating another copy of the string. To avoid this useless duplication, use a const-reference:



```cpp
void f(const String& s) {
  // ...
}
```



##### See also:


* [8 tips to use the String class efficiently](https://cpp4arduino.com/2018/11/21/eight-tips-to-use-the-string-class-efficiently.html)
* “The Missing C++ Course” in [Mastering ArduinoJson](https://arduinojson.org/book/)



## Tip 7: Reduce the size of variables



In situations where every byte counts, you need to make sure that every variable is as small as possible.



For example, suppose you have:



```cpp
int value = 42;
```



If you’re sure that `value` will never exceed 127, you can use a `char` instead of an `int`:



```cpp
char value = 42;
```



Depending on the platform, this can divide the size of this variable by four.



Of course, you save even more memory if it’s an array or a struct. Consider:



```cpp
int values[32]; // 128 bytes on 32-bit architecture
```



You can reduce significantly by using `char` instead:



```cpp
char values[32]; // 32 bytes on any architecture
```



This technique works for local variables, but you can also use it for function parameters. Indeed, when passed by value, function arguments are copied to the stack, so you can save some stack memory by reducing the parameters’ size.



For example, consider changing:



```cpp
void f(int value);
```



to



```cpp
void f(char value);
```



I know it seems to be a small gain, but remember that most functions call other functions, which call other functions… At some point, the stack becomes a big sandwich with the arguments to many functions. If you reduce the size of the arguments, you can save a lot of stack memory.


---


# How to use ArduinoJson with CMake?


Source: https://arduinojson.org/v7/how-to/use-arduinojson-with-cmake/

This page explains how to include the ArduinoJson library in your [CMake](https://cmake.org/) project.



## Method 1: using the `FetchContent` module



If you use CMake 3.14 or above, you can use the built-in [`FetchContent` module](https://cmake.org/cmake/help/latest/module/FetchContent.html).



```cpp
cmake_minimum_required(VERSION 3.14)
project(example)

include(FetchContent)
FetchContent_Declare(ArduinoJson
    GIT_REPOSITORY https://github.com/bblanchon/ArduinoJson.git
    GIT_TAG        v7.4.3
)
FetchContent_MakeAvailable(ArduinoJson)

add_executable(example example.cpp)
target_link_libraries(example ArduinoJson)
```



## Method 2: using `cget`



Alternatively, you can use the [cget package manager](https://cget.readthedocs.io/).



On the command line prompt, run:



```cpp
cget install bblanchon/ArduinoJson
```



Then, in your `CMakeLists.txt`, add the following:



```cpp
cmake_minimum_required(VERSION 3.0)
project(example)

include(cget/cget/cget.cmake)
find_package(ArduinoJson 7 REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example ArduinoJson)
```



## Method 3: using `git submodule`



Lastly, you can rely on Git submodules to fetch the ArduinoJson source.



On the command line prompt, run:



```cpp
git submodule add https://github.com/bblanchon/ArduinoJson.git third-party/ArduinoJson
```



Then, in your `CMakeLists.txt`, add the following:



```cpp
cmake_minimum_required(VERSION 3.0)
project(example)

add_subdirectory(third-party/ArduinoJson)

add_executable(example example.cpp)
target_link_libraries(example ArduinoJson)
```


---


# How to download the latest edition of Mastering ArduinoJson?


Source: https://arduinojson.org/v7/how-to/upgrade-ebook/

If you already own the ebook edition of [Mastering ArduinoJson](https://arduinojson.org/book/), you can upgrade to the latest edition for free.


* If you purchased on arduinojson.org, you should have received an email inviting you to download the new edition. If not, search your Inbox for a mail titled “Thank you for your purchase!”; the download link you received back then should still work.
* If you purchased on Amazon Kindle, you’d get the new version if you opted for the automatic updates. Otherwise, you must go to [Manage Your Content and Devices](https://www.amazon.com/hz/mycd/myx), and you should see an “update available” button.
* If you purchased on LeanPub, log in and go to [Your Library](https://leanpub.com/user_dashboard/library).



If none of these options work, please send me an email; you’ll find my address in the last chapter of the book.



Thanks again for purchasing [Mastering ArduinoJson](https://arduinojson.org/book/), your support means a lot!


---


# How to get the library’s version?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/how-to/get-library-version/

ArduinoJson comes with the four following macros:



```cpp
#define ARDUINOJSON_VERSION "7.4.3"
#define ARDUINOJSON_VERSION_MAJOR 7
#define ARDUINOJSON_VERSION_MINOR 4
#define ARDUINOJSON_VERSION_REVISION 3
```



Here is how you can print ArduinoJson’s version:



```cpp
Serial.print("Using ArduinoJson version ");
Serial.println(ARDUINOJSON_VERSION);  // 7.4.3
```



Here is how you can test that the expected version is installed:



```cpp
#if ARDUINOJSON_VERSION_MAJOR!=7 || ARDUINOJSON_VERSION_MINOR<4
#error ArduinoJson 7.4+ is required
#endif
```



## See also:


* [`ARDUINOJSON_VERSION`](https://arduinojson.org/v7/api/misc/version/)


---


# Can I automatically serialize an object?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/faq/automatically-serialize-an-object/

ArduinoJson users frequently ask if there is a way to serialize an object automatically.



Let’s say you defined a `struct` that contains the configuration of your program, and you want to restore this object when the application starts. If we look at [JsonConfigFile.ino](https://arduinojson.org/v7/example/config/), we see that we have to write some code to convert the `struct` to a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/). Isn’t there a simpler way?



We can do that in C#, Java, Python, JavaScript, etc. so it must be possible with ArduinoJson, right?



**Unfortunately, no. It’s not possible to do that**.



Let me explain why I cannot implement this feature, and why I don’t really miss it.



## No reflection/introspection in C++



The main reason why we can’t automatically serialize an object is that C++ lacks the reflection feature that other languages have. Indeed, there is no facility in C++ that allows a program to enumerate the members of an object.



There are some workarounds out there, but there are not pretty:


* [Boost Fusion](https://www.boost.org/doc/libs/1_67_0/libs/fusion/doc/html/fusion/adapted.html)
* [CAMP](https://github.com/tegesoft/camp)
* [Ponder](https://github.com/billyquith/ponder)
* [RTTR](https://www.rttr.org/)



If you try to implement one of these solutions, you’ll quickly realize that they require more effort than writing the original conversion code.



## No standard container on Arduino



Even if I decide to go with a solution like [RTTR](https://www.rttr.org/), there is another road-block: Arduino doesn’t provide a standard container like [`std::vector`](https://en.cppreference.com/w/cpp/container/vector) or [`std::list`](https://en.cppreference.com/w/cpp/container/list). Therefore, we have no common way to represent a collection of objects.



Sure, some cores embed libstdc++, so [`std::vector`](https://en.cppreference.com/w/cpp/container/vector) and [`std::list`](https://en.cppreference.com/w/cpp/container/list) are available on some platforms, but I could not write something portable.



## Deserialization is non trivial



Admitting that automatic serialization was possible, you must also think of the reverse operation: deserialization. I invite you to write a deserialization function for a complex object. You’ll realize that there are several decisions that you’ll have to make:


* What if a member is missing?
* What if there is an extra member?
* What if a value has the wrong type?
* What if an array is too large?
* What if a value is out of range?



I don’t think a library can automatically make these decisions for you. Even if the library offered options to configure the behavior for each member, you’d end up with something more complex than the hand-written function.



## Why I don’t miss the feature



I’ve worked a lot with C#, and I used NewtonSoft Json.NET a few times. Admittedly, the conversion from structs to JSON is convenient.



However, my experience showed that serializing structs directly encourages programmers to serialize the business objects, thereby coupling the serialization format to the business model. Even in C#, I used to add an extra layer of indirection between my business logic and my serialization code, to be sure I can change one without breaking the other. For example, it allowed my app to be backward compatible with old configuration files.



The code to glue the business model with the serialization structs is very similar to the code you write with ArduinoJson to fill the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). One has to write this code anyway, so why bother.


---


# Why should I create a separate config object?Why can’t I use JsonDocument directly?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/faq/why-should-i-create-a-separate-config-object/

As [JsonConfigExample.ino](https://arduinojson.org/v7/example/config/) shows, it’s important to use custom structures to store the state of your application.



Many users are puzzled by this advice. Why should I create my own structure when [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) already contains all I need? Indeed, this seems like a perfect solution: the comfort of a loosely typed JavaScript object model inside a C++ program. Unfortunately, this solution is far from perfect; let’s see why in detail.



## Reason 1: overhead



Passing through the JSON object model is less efficient regarding:


1. **Memory usage**: a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is much larger than an `int`, for example.
2. **Execution speed**: accessing a member of a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is much slower than accessing a `struct` member.
3. **Code size**: accessing a member of a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) generates more instructions than accessing a `struct` member.



## Reason 2: type safety



Once your data is captured in C++ structures, you can rely on the security provided by the type system; you don’t have to check that such member is present and has the right type, which allows for detecting bugs at compile time.



## Reason 3: coupling



The way information is serialized is an implementation detail; the rest of the program should not depend on it.



Moreover, you should not spread ArduinoJson everywhere in your code beacuse you would be too dependent on ArduinoJson. What if you want to use another library? What if I change the API again? You don’t want to be at the mercy of a library developer, even me.



## See also


* [JsonConfigExample.ino](https://arduinojson.org/v7/example/config/) demonstrates this technique
* The case study “Configuration in SPIFFS” in [Mastering ArduinoJson](https://arduinojson.org/book/) does the same with a complex configuration with nested objects.


---


# Why does the document take up so much RAM?
 | ArduinoJson 7

Source: https://arduinojson.org/v7/faq/why-does-the-document-take-up-so-much-ram/

Every once in a while, I receive a message from an unhappy user who finds that the library consumes too much memory. Invariably, the person wants to store an array of integers in a JSON document and is shocked by the size computed by the [ArduinoJson Assistant](https://arduinojson.org/v7/assistant/). The rationale is always the same: “My input file is 2KB long, how could it take 3KB of RAM?”



Let me illustrate the issue with an example. Here is a JSON document containing an array of integers:



```cpp
[1,2,3,4]
```



In its serialized form, this JSON document takes 9 bytes. Yet, the [ArduinoJson Assistant](https://arduinojson.org/v7/assistant/) tells us we need a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) with a capacity of 64 bytes!



Indeed, from this angle ArduinoJson (and all other JSON libraries) looks terrible. Let’s see another example, again with four integers:



```cpp
[-9223372036854775808,-9223372036854775808,-9223372036854775808,-9223372036854775808]
```



Now, the serialized form is 85 bytes, but the Assistant still says we need 64 bytes. ArduinoJson certainly looks better from that angle.



As you can see, the serialized size depends on the values, whereas the size in RAM doesn’t. Therefore, comparing these two measures doesn’t give a good appreciation of the library’s memory efficiency.



What would be a good measure, then? We could compare it with the RAM a C program uses to store the same information. For example, the following array takes 16 bytes on most platforms:



```cpp
int values[4];
```



Right, but ArduinoJson stores 64-bit integers, so a closer approximation would be:



```cpp
int64_t values[4];
```



This array takes 32 bytes; it’s still twice less than what ArduinoJson requires!



Indeed, but programs don’t always insert the values sequentially. For example, a program could do that:



```cpp
doc["array1"].add(1);
doc["array2"].add(1);
doc["array1"].add(2);
```



Even though 1 and 2 appear next to each other in the serialized form, they were not added together in the document. If the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) stored these values sequentially, it would have to reallocate “array1” on the third line to make room for the new value. Sure, I could implement this behavior, but it would complexify the code and significantly increase the library size.



Of course, a similar problem occurs if the program removes an element:



```cpp
doc["array1"].add(1);
doc["array1"].add(2);
doc["array1"].remove(0);
```



If elements were stored sequentially, ArduinoJson would have to move the elements. Again, doable but not ideal.



That’s why ArduinoJson stores arrays (and objects) as a linked list. If you want to replicate that in a C++ program, it would look like so:



```cpp
struct Node {
   int64_t* value;
   Node* next;
};

Node nodes[4];
```



Check by yourself, and you’ll see that `sizeof(nodes)` is 64.



Admittedly, 64 bytes to store 1, 2, 3, and 4 is too much, but it’s only a problem if you have many numbers. If that’s your case, you’re probably trying to store binary data; something JSON is notoriously bad for. Instead, you should consider using a binary format. You can watch my [*Serialization and JSON on Arduino* presentation on YouTube](https://youtu.be/hAB4TdX8dwM).



Anyway, this excessive RAM usage should be very limited in time. Indeed, ArduinoJson is a serialization library; it never pretended to be a replacement for “vector". [`JsonDocument`](/v7/api/jsondocument/) is designed to be a throw-away data structure only used during serialization. If you need an array of integers in your program, extract the values before discarding the [`JsonDocument`](/v7/api/jsondocument/) like so:



```cpp
int values[4];

void readConfig(File file) {
  JsonDocument doc;
  deserializeJson(doc, file);
  copyArray(doc["values"], values);
}
```



This code snippet demonstrates how to best use the library: the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is short-lived, so memory consumption is less of a problem.



There are other reasons why you should follow this pattern in your program. Please read [Why must I create a separate config object? Why can’t I use `JsonDocument` directly?](https://arduinojson.org/v7/faq/why-does-the-document-take-up-so-much-ram/)


---


# JsonDocument::isNull()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/isnull/

## Description



[`JsonDocument::isNull()`](https://arduinojson.org/v7/api/jsondocument/isnull/) tells whether the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is empty, i.e. if the document’s root is null.



## Signature



```cpp
bool isNull() const;
```



## Return value



[`JsonDocument::isNull()`](https://arduinojson.org/v7/api/jsondocument/isnull/) returns a `bool` that tells if the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is empty:


* `true` if the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is empty,
* `false` if the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) contains something.



## Example



### Result is true



```cpp
JsonDocument doc;
doc.isNull(); // true
serializeJson(doc, Serial); // prints "null"
```



### Result is false



```cpp
JsonDocument doc;
doc.to<JsonArray>();
doc.isNull(); // false
serializeJson(doc, Serial); // prints "[]"
```



## See also


* [`JsonDocument::clear()`](https://arduinojson.org/v7/api/jsondocument/clear/)


---


# JsonDocument::to<T>()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/to/

## Description



Clears the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) and converts it to the specified type.



## Signature



```cpp
JsonArray   to<JsonArray>();
JsonObject  to<JsonObject>();
JsonVariant to<JsonVariant>();
```



## Example



```cpp
// create the JsonDocument
JsonDocument doc;

// convert it to a JsonObject
JsonObject root = doc.to<JsonObject>();

// set values
root["hello"] = "world";
```



> > ### Invalidates references



> Because this function releases memory owned by the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), it invalidates all references acquired before.



> Example:



> ```cpp
JsonDocument doc;

JsonObject root1 = doc.to<JsonObject>();

JsonObject root2 = doc.to<JsonObject>();

// Don't use root1 here, because it's dangling!
> ```



## See also


* [`JsonDocument::as<T>()`](https://arduinojson.org/v7/api/jsondocument/as/)


---


# JsonDocument::as<T>()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/as/

## Description



Casts [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to the specified type.



Unlike [`JsonDocument::to<T>()`](https://arduinojson.org/v7/api/jsondocument/to/), this function doesn’t change the content of the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## Signature



```cpp
bool               as<bool>() const;

float              as<float>() const;
double             as<double>() const;

signed char        as<signed char>() const;
unsigned char      as<unsigned char>() const;
signed int         as<signed int>() const;
unsigned int       as<unsigned int>() const;
signed short       as<signed short>() const;
unsigned short     as<unsigned short>() const;
signed long        as<signed long>() const;
unsigned long      as<unsigned long>() const;
unsigned long long as<unsigned long long>() const;   // ⚠️ may require ARDUINOJSON_USE_LONG_LONG
signed long long   as<signed long long>() const;     // ⚠️ may require ARDUINOJSON_USE_LONG_LONG

const char*        as<const char*>() const;
String             as<String>() const;               // ⚠️ behavior differs slightly (see below)
std::string        as<std::string>() const;          // ⚠️ behavior differs slightly (see below)

JsonArray          as<JsonArray>();
JsonObject         as<JsonObject>();
JsonVariant        as<JsonVariant>();

JsonArrayConst     as<JsonArrayConst>() const;
JsonObjectConst    as<JsonObjectConst>() const;
JsonVariantConst   as<JsonVariantConst>() const;

TEnum              as<TEnum>() const;  // alias of as<int>()
T                  as<T>() const;      // calls user-defined converter
```



## Return value



This function returns a reference to the root of the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



If the actual type of the root doesn’t match the requested type, this function returns a null reference. For example, suppose the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is an array, if you call `JsonDocument::as<JsonObject>()`, it will return a null [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



## `as<String>()` and `as<std::string>()`



`JsonDocument::as<T>()` behaves slightly differently when `T` is a string object:


* if the value is a string, it returns this string (nothing special here);
* otherwise, it returns the JSON representation.



Here are a few examples:



| Value | `as<String>()` |
| --- | --- |
| `"hello world"` | `"hello world"` |
| `true` | `"true"` |
| `false` | `"false"` |
| `42` | `"42"` |
| `[1,2,3]` | `"[1,2,3]"` |
| `{"key":"value"}` | `"{\"key\":\"value\"}"` |
| `null` | `"null"` |



If you don’t want this behavior, use `as<const char*>()` instead.



## Integer overflows



`JsonDocument::as<T>()` is aware of integer overflows and only returns a value if it can fit in the specified type.



For example, if the value contains `512`, `as<char>()` returns `0`, but `as<int>()` returns `512`.



## User-defined types



`JsonDocument::as<T>()` supports user-defined types by calling `convertFromJson()`. For example, to support [`tm`](https://en.cppreference.com/w/c/chrono/tm), you must define the following function:



```cpp
void convertFromJson(JsonVariantConst src, tm& dst) {
  strptime(src.as<const char*>(), "%FT%TZ", &dst);
}
```



For more information about custom converters, please read [the article dedicated to ArduinoJson 6.18.0](https://arduinojson.org/news/2021/05/04/version-6-18-0/).



## Example



```cpp
JsonDocument doc;
deserializeJson(doc, "{\"key\":\"value\")");

// get the JsonObject in the JsonDocument
JsonObject root = doc.as<JsonObject>();

// get the value in the JsonObject
const char* value = root["key"];
```



## See also


* [`JsonDocument::to<T>()`](https://arduinojson.org/v7/api/jsondocument/to/)
* [`JsonVariant::as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/)


---


# JsonDocument::add()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/add/

## Description



When the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) contains an array, `JsonDocument::add()` appends a value to the array.



When the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) contains a value that is not an array, `JsonDocument::add()` does nothing.



When the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is empty, `JsonDocument::add()` converts the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to an array containing one element.



This feature allows creating an array without calling [`JsonDocument::to<JsonArray()`](https://arduinojson.org/v7/api/jsondocument/to/). For example the two following snippets are equivalent:



```cpp
JsonArray arr = doc.to<JsonArray>();
arr.add(42);
```



```cpp
doc.clear();
doc.add(42);
```



## Signatures



```cpp
bool add(bool value);

bool add(float value);
bool add(double value);

bool add(signed char value);
bool add(signed long value);
bool add(signed int value);
bool add(signed short value);
bool add(unsigned char value);
bool add(unsigned long value);
bool add(unsigned int value);
bool add(unsigned short value);

bool add(const char (&value)[N]);           // stores a pointer
bool add(const char *value);                // stores a copy
bool add(const __FlashStringHelper *value); // stores a copy

bool add(const String &value);       // stores a copy
bool add(const std::string &value);  // stores a copy
bool add(const Printable& value);    // stores a copy
bool add(std::string_view value);    // stores a copy

bool add(JsonArray array);          // stores a deep copy
bool add(JsonObject object);        // stores a deep copy
bool add(JsonVariant variant);      // stores a deep copy
bool add(const JsonDocument& doc);  // stores a deep copy

bool add(TEnum value);  // alias of add(int)
bool add(T value);      // calls user-defined function

JsonArray   add<JsonArray>() const;    // adds a new empty array
JsonObject  add<JsonObject>() const;   // adds a new empty object
JsonVariant add<JsonVariant>() const;  // adds a new null variant
```



## Arguments



`value`: the value of to append to the array, it can be any type supported by ArduinoJson.



All values are stored by copy, except string literals which are stored by address.



The string copy policy has changed in [ArduinoJson 7.3](https://arduinojson.org/news/2024/12/29/arduinojson-7-3/).



## Return value



`JsonDocument::add()` returns a `bool` that tells whether the operation was successful or not:


* `true` if the value was successfully added.
* `false` if there was not enough memory in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## Example



```cpp
JsonDocument doc;
array.add("hello"); // null -> ["hello"]
array.add(3.14156); // ["hello"] -> ["hello",3.14156]
serializeJson(doc, Serial);
```



will write



```cpp
["hello",3.14156]
```



## See also


* [`JsonArray::add()`](https://arduinojson.org/v7/api/jsonarray/add/)


---


# JsonDocument::clear()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/clear/

## Description



`JsonDocument::clear()` empties [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



Unlike [`JsonArray::clear()`](https://arduinojson.org/v7/api/jsonarray/clear/) and [`JsonObject::clear()`](https://arduinojson.org/v7/api/jsonobject/clear/), this function doesn’t need to perform any housekeeping so it’s a bit faster.



You **don’t** need to call `JsonDocument::clear()`:


* after creating the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)
* before destroying the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)
* before calling [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/) or [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/)
* before calling [`JsonDocument::to<T>()`](https://arduinojson.org/v7/api/jsondocument/to/)



These functions already clear the memory pool, so you **don’t** need to call [`JsonDocument::clear()`](https://arduinojson.org/v7/api/jsondocument/clear/).



Every reference ([`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), or [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/)) acquired before calling [`clear()`](https://arduinojson.org/v7/api/jsondocument/clear/) is invalidated.



## Signature



```cpp
void clear();
```



## See also


* [`JsonDocument::isNull()`](https://arduinojson.org/v7/api/jsondocument/isnull/)
* [`JsonDocument::to<T>()`](https://arduinojson.org/v7/api/jsondocument/to/)


---


# JsonDocument::containsKey()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/containskey/

> > ## Deprecated



> `containsKey()` is deprecated since ArduinoJson 7.2 and you must use [`is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/) instead.



> ```cpp
// Before
if (doc.containsKey("value")) {
  int value = doc["value"];
  // ...
}

// After
if (doc["value"].is<int>()) {
  int value = doc["value"];
  // ...
}
> ```



> If you want to check that a key exists regardless of its type, you can use `is<JsonVariant>()`.



## Description



[`JsonDocument::containsKey()`](https://arduinojson.org/v7/api/jsondocument/containskey/) tests whether a key exists at the root of the document.



This function only works if the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) contains an object. It returns `false` if the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is empty, or if it contains an array.



This function can (and should) be avoided most of the time. See below.



## Signature



```cpp
bool containsKey(const char* key) const;
bool containsKey(const String& key) const;
bool containsKey(const std::string& key) const;
bool containsKey(const __FlashStringHelper& key) const;
bool containsKey(std::string_view key) const;
```



## Arguments



`key`: the key to look for.



## Return value



[`JsonDocument::containsKey()`](https://arduinojson.org/v7/api/jsondocument/containskey/) returns a `bool` that tells whether the key was found or not:


* `true` if the key is present in the document
* `false` if the key is absent of the document



## Example



```cpp
JsonDocument doc;
doc["city"] = "Paris";

bool hasCity = doc.containsKey("city"); // true
bool hasCountry = doc.containsKey("country"); // false
```



## Avoid this function when you can!



This function can (and should) be avoided most of the time.



Because ArduinoJson implements the [Null Object Pattern](https://en.wikipedia.org/wiki/Null_Object_pattern), it is always safe to read the object: if the key doesn’t exist, it returns an empty value. For example:



```cpp
JsonDocument doc;
doc["city"] = "Paris";

if (doc.containsKey("city")) {
  const char* city = doc["city"];
  Serial.println(city);
}
```



Can be written like this:



```cpp
JsonDocument doc;
doc["city"] = "Paris";

const char* city = doc["city"];
if (city)
  Serial.println(city);
```



This version should lead to a **smaller** and **faster** code since it only does the lookup once.



## How to test nested keys?



You cannot test the presence of nested keys with `containsKey()` but, as explained above, it’s safe to read the object anyway.



For example, when [Weather Underground](http://www.wunderground.com) returns an error like:



```cpp
{
  "response": {
    "version": "0.1",
    "termsofService": "http://www.wunderground.com/weather/api/d/terms.html",
    "features": {
      "conditions": 1
    },
    "error": {
      "type": "querynotfound",
      "description": "No cities match your search query"
    }
  }
}
```



You should not try to call `containsKey("response")`, `containsKey("error")` and `containsKey("description")`. Instead, just get the value and test if it’s null:



```cpp
const char* error = doc["response"]["error"]["description"];
if (error) {
  Serial.println(error);
  return;
}
```



## See also


* [`JsonObject::containsKey()`](https://arduinojson.org/v7/api/jsonobject/containskey/)
* [`JsonVariant::containsKey()`](https://arduinojson.org/v7/api/jsonvariant/containskey/)


---


# JsonDocument::operator[]
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/subscript/

## Description



`JsonDocument::operator[]` gets, replaces, or adds a value in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



Depending on the argument type the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) can be used like a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) or a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



If you use this operator to set a value in an empty [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), it automatically converts [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) to the appropriate type. This feature allows creating object without calling [`JsonDocument::to<JsonObject>()`](https://arduinojson.org/v7/api/jsondocument/to/). Here is an example:



```cpp
JsonDocument doc;
doc["wifi"]["SSID"] = "TheBatCave";
```



The two lines above create the following document:



```cpp
{
    "wifi": {
        "SSID": "TheBatCave"
    }
}
```



All values are stored by copy, except string literals which are stored by address.



The string copy policy has changed in [ArduinoJson 7.3](https://arduinojson.org/news/2024/12/29/arduinojson-7-3/).



If you need to know if the insertion succeed, use [`JsonVariant::set()`](https://arduinojson.org/v7/api/jsonvariant/set/) instead.



## Signatures



```cpp
// Shared with JsonArray
ElementProxy operator[](size_t index);

// Shared with JsonArrayConst
JsonVariantConst operator[](size_t index) const;

// Shared with JsonObject
MemberProxy operator[](const char (&key)[N]);
MemberProxy operator[](const char* key);
MemberProxy operator[](const String& key);
MemberProxy operator[](const std::string& key);
MemberProxy operator[](const __FlashStringHelper* key);
MemberProxy operator[](std::string_view key);

// Shared with JsonObjectConst
JsonVariantConst operator[](const char* key) const;
JsonVariantConst operator[](const String& key) const;
JsonVariantConst operator[](const std::string& key) const;
JsonVariantConst operator[](const __FlashStringHelper* key) const;
JsonVariantConst operator[](std::string_view key) const;
```



## Arguments



`index`: the index in the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



`key`: the key in the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/)



## Return value



A proxy class that allows using the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) as an array or a dictionary. If this concept of proxy class is unfamiliar to you, just think of a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) instead.



Please see [`JsonArray::operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/) and [`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/) for explanations about `ElementProxy` and `MemberProxy`.



## Example



```cpp
JsonDocument doc;

deserializeJson(doc, "{\"hello\":\"world\"}");
const char* hello = variant["hello"];

deserializeJson(doc, "[\"hello\",\"world\"]");
const char* world = variant[1];
```



## See also


* [`JsonArray::operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/)
* [`JsonArrayConst::operator[]`](https://arduinojson.org/v7/api/jsonarrayconst/subscript/)
* [`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/)
* [`JsonObjectConst::operator[]`](https://arduinojson.org/v7/api/jsonobjectconst/subscript/)


---


# JsonDocument::overflowed()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/overflowed/

## Description



`JsonDocument::overflowed()` tells if the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) failed to store a value.



Storing a value can fail for two reasons:


1. The heap is exhausted: there is not enough free memory to expand the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).
2. The maximum number of slots was reached (see table below).
3. One or more string exceeds the maxium length (see table below).



| CPU architecture | Max slots | Max string length |
| --- | --- | --- |
| 8-bit | 255 | 255 |
| 32-bit | 65,635 | 65,635 |
| 64-bit | 4,294,967,294 | 65,635 |



You can increase the number of slots by changing [`ARDUINOJSON_SLOT_ID_SIZE`](https://arduinojson.org/v7/config/slot_id_size/).
 You can increase the maximum string length by changing [`ARDUINOJSON_STRING_LENGTH_SIZE`](https://arduinojson.org/v7/config/string_length_size/).
 [ArduinoJson Assitant](https://arduinojson.org/v7/assistant/) is aware of these limitations and will tell you if you need to change these settings.



Calling [`JsonDocument::clear()`](https://arduinojson.org/v7/api/jsondocument/clear/) clears the “overflowed” flag.



## Signature



```cpp
bool overflowed() const;
```



## Return value



`JsonDocument::overflowed()` returns a `bool` that tells if the memory pool overflowed


* `false` when all values were stored successfully.
* `true` when at least one value could not be stored.



## See also


* [How to deserialize a very large document?](https://arduinojson.org/v7/how-to/deserialize-a-very-large-document/)
* [How to reduce memory usage?](https://arduinojson.org/v7/how-to/reduce-memory-usage/)


---


# JsonDocument::is<T>()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/is/

## Description



`JsonDocument::is<T>()` tests if the document is currently holding a value of type `T`.



## Signatures



```cpp
bool is<bool>() const;

bool is<float>() const;
bool is<double>() const;

bool is<signed char>() const;
bool is<unsigned char>() const;
bool is<signed int>() const;
bool is<unsigned int>() const;
bool is<signed short>() const;
bool is<unsigned short>() const;
bool is<signed long>() const;
bool is<unsigned long>() const;
bool is<unsigned long long>() const;   // ⚠️ may require ARDUINOJSON_USE_LONG_LONG
bool is<signed long long>() const;     // ⚠️ may require ARDUINOJSON_USE_LONG_LONG

bool is<const char*>() const;
bool is<String>() const;
bool is<std::string>() const;

bool is<JsonArray>() const;
bool is<JsonObject>() const;
bool is<JsonVariant>() const;

bool is<JsonArrayConst>() const;
bool is<JsonObjectConst>() const;
bool is<JsonVariantConst>() const;

bool is<TEnum>() const; // alias of as<int>()
bool is<T>() const;     // calls user-defined function
```



## Return value


* `true` if the document is currently holding a value of type `T`,
* `false` if not



## JSON types vs. C++ types



Different C++ types can store the same JSON value, so `is<T>()` can return `true` for several `T`s. For example, `is<float>()` always returns the same value as `is<double>()` .



The table below gives the correspondence between the JSON type and the C++ types:



| JSON type | `T` |
| --- | --- |
| Floating point | `float`, `double` |
| Integer | `int`, `short`, `long`, `long long` |
| String | `const char*`, `char*` |
| Boolean | `bool` |
| Array | [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) |
| Object | [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) |



Caution: `is<float>()` and `is<double>()` return `true` for integers too.



## Integer overflows



`JsonDocument::is<T>()` is aware of integer overflows and only returns `true` if the specified type can store the value.



For example if the variant contains `512`, `is<char>()` returns `false`, but `is<int>()` return `true`.



## User-defined types



`JsonDocument::is<T>()` supports user-defined types by calling `canConvertFromJson()`. For example, to support an hypothetical `Complex` class, we could write:



```cpp
bool canConvertFromJson(JsonVariantConst src, const Complex&) {
  return src["real"].is<double>() && src["imag"].is<double>();
}
```



The second parameter of `canConvertFromJson()` is required to trigger [ADL](https://en.cppreference.com/w/cpp/language/adl) but must not be used by the function.



## Example



```cpp
JsonDocument doc;
doc.set(42);
doc.is<int>();         // true
doc.is<double>();      // true
doc.is<const char*>(); // false
doc.is<JsonArray>();   // false
```



## See also


* [`JsonDocument::as<T>()`](https://arduinojson.org/v7/api/jsondocument/as/)
* [`JsonVariant::is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/)


---


# JsonDocument::nesting()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/nesting/

## Description



`JsonDocument::nesting()` returns the depth (i.e., the nesting level) of the document.



You can use use the result for [`DeserializationOption::NestingLimit`](https://arduinojson.org/v7/api/json/deserializejson/#nesting-limit)



| JSON | Nesting |
| --- | --- |
| `null` | 0 |
| `"string"` | 0 |
| `[0]` | 1 |
| `[[0]]` | 2 |
| `{"a":0}` | 1 |
| `{"a":{"b":0}}` | 2 |
| `{"a":{"b":[0]}}` | 3 |



## Signature



```cpp
size_t nesting() const;
```



## Example



```cpp
const char* json = "{\"list\":[{\"value\":\"1\"}]}";
JsonDocument doc;
deserializeJson(doc, json);
Serial.println(doc.nesting());  // prints 3
```


---


# JsonDocument::remove()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/remove/

## Description



Depending on the type of its argument, `JsonDocument::remove()` either:


* removes the element at the specified index from the root array
* removes the member with the specified key in the root object



In other words, `JsonDocument::remove()` behaves like [`JsonArray::remove()`](https://arduinojson.org/v7/api/jsonarray/remove/) or [`JsonObject::remove()`](https://arduinojson.org/v7/api/jsonobject/remove/).



`JsonDocument::remove()` does nothing if the root value doesn’t match the expected type. For example, if you call `remove("key")` on an array, the document won’t change.



## Signature



```cpp
void remove(size_t index) const;

void remove(const char* key) const;
void remove(const __FlashStringHelper* key) const;
void remove(const String& key) const;
void remove(const std::string& key) const;
void remove(std::string_view key) const;
```



## Arguments



`index`: the zero-based position of the array element to remove.



`key`: the key of the object member to remove



## Examples



### With an array



```cpp
JsonDocument doc;
doc.add("Ned");
doc.add("Maude");
doc.add("Rod");
doc.add("Todd");
doc.remove(1);  // removes "Maude"
serializeJson(doc, Serial);  // prints ["Ned","Rod","Todd"]
```



### With an object



```cpp
JsonDocument doc;
doc["name"] = "Homer";
doc["age"] = 38;
doc["weight"] = 239;
doc.remove("age");
serializeJson(doc, Serial);  // prints {"name":"Homer","weight":239}
```



## See also


* [`JsonArray::remove()`](https://arduinojson.org/v7/api/jsonarray/remove/)
* [`JsonObject::remove()`](https://arduinojson.org/v7/api/jsonobject/remove/)
* [`JsonVariant::remove()`](https://arduinojson.org/v7/api/jsonvariant/remove/)


---


# JsonDocument::set()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/set/

## Description



`JsonDocument::set()` replaces the root value of the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## Signatures



```cpp
bool set(bool value) const;

bool set(float value) const;
bool set(double value) const;

bool set(signed char value) const;
bool set(signed long value) const;
bool set(signed int value) const;
bool set(signed short value) const;
bool set(unsigned char value) const;
bool set(unsigned long value) const;
bool set(unsigned int value) const;
bool set(unsigned short value) const;

bool set(const char (&value)[N]) const;            // stores a pointer
bool set(const char *value) const;                 // stores a copy
bool set(const __FlashStringHelper *value) const;  // stores a copy

bool set(const String &value) const;       // stores a copy
bool set(const std::string &value) const;  // stores a copy
bool set(const Printable& value) const;    // stores a copy
bool set(std::string_view value) const;    // stores a copy

bool set(JsonArray array) const;         // stores a deep copy
bool set(JsonObject object) const;       // stores a deep copy
bool set(JsonVariant variant) const;     // stores a deep copy
bool set(const JsonVariant& doc) const;  // stores a deep copy

bool set(TEnum value) const;  // alias of set(int)
bool set(T value) const;      // calls user-defined function
```



## Arguments



`value`: the new value of the document, it can be any type supported by ArduinoJson or a user-defined type if you define [custom converters](https://arduinojson.org/news/2021/05/04/version-6-18-0/).



All values are stored by copy, except string literals which are stored by address.



The string copy policy has changed in [ArduinoJson 7.3](https://arduinojson.org/news/2024/12/29/arduinojson-7-3/).



## Return value



`JsonDocument::set()` returns a `bool` that tells whether the operation was successful or not:


* `true` if the value operation was successful.
* `false` if there was not enough room in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## User-defined types



`JsonDocument::set()` supports user-defined types by calling `convertToJson()`. For example, to support [`tm`](https://en.cppreference.com/w/c/chrono/tm), you must define:



```cpp
bool convertToJson(const tm& src, JsonVariant dst) {
  char buf[32];
  strftime(buf, sizeof(buf), "%FT%TZ", &src);
  return dst.set(buf);
}
```



## Example



```cpp
JsonDocument doc;
doc.set("hello world")
serializeJson(doc, Serial);
```



The above program produces the following output:



```cpp
"hello world"
```



## See also


* [`JsonVariant::set()`](https://arduinojson.org/v7/api/jsonvariant/set/)


---


# JsonDocument::shrinkToFit()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/shrinktofit/

## Description



To improve performance, ArduinoJson allocates more memory than necessary. This function releases the unused memory.



This function is automatically called by [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/).



You probably don’t need to call this function.



## Example



```cpp
JsonDocument doc;
doc["hello"] = "world";
doc.shrinkToFit();  // release overallocated memory
```


---


# JsonDocument::size()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsondocument/size/

## Description



If the root of the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is an object, `JsonDocument::size()` returns the number of key-value pairs in the object (same as [`JsonObject::size()`](https://arduinojson.org/v7/api/jsonobject/size/))



If the root of the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) is an array, `JsonDocument::size()` returns the number of elements in the array (same as [`JsonArray::size()`](https://arduinojson.org/v7/api/jsonarray/size/))



In all other cases, `JsonDocument::size()` returns `0`



Internally, this function walks a linked-list to count the elements, so its [time complexity](https://en.wikipedia.org/wiki/Time_complexity) is *O(n)*.



## Signature



```cpp
size_t size() const;
```



## Example



```cpp
JsonDocument doc;
object["hello"] = "world";
Serial.println(object.size()); // 1
```



## See also


* [`JsonArray::size()`](https://arduinojson.org/v7/api/jsonarray/size/)
* [`JsonObject::size()`](https://arduinojson.org/v7/api/jsonobject/size/)
* [`JsonVariant::size()`](https://arduinojson.org/v7/api/jsonvariant/size/)


---


# JSON
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/json/

### [`deserializeJson()`](https://arduinojson.org/v7/api/json/deserializejson/)
 Deserialize a JSON document into a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).







### [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/)
 Serializes a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) into a minified JSON document







### [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/)
 Serializes a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) into a prettyfied JSON document







### [`measureJson()`](https://arduinojson.org/v7/api/json/measurejson/)
 Computes the size of the document that [`serializeJson()`](https://arduinojson.org/v7/api/json/serializejson/) produces







### [`measureJsonPretty()`](https://arduinojson.org/v7/api/json/measurejsonpretty/)
 Computes the size of the document that [`serializeJsonPretty()`](https://arduinojson.org/v7/api/json/serializejsonpretty/) produces


---


# JsonVariantConst
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariantconst/

## Description



[`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) is a read-only version of [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/). Like [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), it points to a variant in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). The difference is that [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) provided only read access to the variant and doesn’t allow modifying the value. It’s also twice smaller because it doesn’t contain a pointer to the memory pool.



Use this [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) in place of [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) when you want to enforce immutability or when you work with a `const` [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## Member functions


* [`as<T>()`](https://arduinojson.org/v7/api/jsonvariantconst/as/)
* [`containsKey()`](https://arduinojson.org/v7/api/jsonvariantconst/containskey/) (deprecated)
* [`isNull()`](https://arduinojson.org/v7/api/jsonvariantconst/isnull/)
* [`is<T>()`](https://arduinojson.org/v7/api/jsonvariantconst/is/)
* [`nesting()`](https://arduinojson.org/v7/api/jsonvariantconst/nesting/)
* [`operator|`](https://arduinojson.org/v7/api/jsonvariantconst/or/)
* [`operator[]`](https://arduinojson.org/v7/api/jsonvariantconst/subscript/)
* [`size()`](https://arduinojson.org/v7/api/jsonvariantconst/size/)



[`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) also support the comparison operators `==`, `!=`, `<`, `<=`, `>`, and `>=`.



## See also


* [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/)
* [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/)
* [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/)


---


# JsonVariantConst::as<T>()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariantconst/as/

## Description



`JsonVariantConst::as<T>()` casts the value pointed by the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) to the specified type.



## Signatures



```cpp
bool               as<bool>() const;

float              as<float>() const;
double             as<double>() const;

signed char        as<signed char>() const;
unsigned char      as<unsigned char>() const;
signed int         as<signed int>() const;
unsigned int       as<unsigned int>() const;
signed short       as<signed short>() const;
unsigned short     as<unsigned short>() const;
signed long        as<signed long>() const;
unsigned long      as<unsigned long>() const;
unsigned long long as<unsigned long long>() const;  // ⚠️ may require ARDUINOJSON_USE_LONG_LONG
signed long long   as<signed long long>() const;    // ⚠️ may require ARDUINOJSON_USE_LONG_LONG

const char*        as<const char*>() const;
String             as<String>() const;       // ⚠️ behavior differs slightly (see below)
std::string        as<std::string>() const;  // ⚠️ behavior differs slightly (see below)

JsonArray          as<JsonArray>() const;    // ⚠️ always returns null
JsonObject         as<JsonObject>() const;   // ⚠️ always returns null
JsonVariant        as<JsonVariant>() const;  // ⚠️ always returns null

JsonArrayConst     as<JsonArrayConst>() const;
JsonObjectConst    as<JsonObjectConst>() const;
JsonVariantConst   as<JsonVariantConst>() const;

TEnum              as<TEnum>() const;  // alias of as<int>()
T                  as<T>() const;      // calls user-defined converter
```



## Return value



`JsonVariantConst::as<T>()` returns the value pointed by the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) cast to the specified type.



This function returns a default value if the cast is not possible. The default value is:


* `0` for numerical types
* `NULL` for `const char*`
* A null reference for [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) and [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



To change the default value, you need to use [`JsonVariantConst::operator|`](https://arduinojson.org/v7/api/jsonvariantconst/or/) instead.



## `as<String>()` and `as<std::string>()`



`JsonVariantConst::as<T>()` behaves slightly differently when `T` is a string object:


* if the value is a string, it returns this string (nothing special here);
* otherwise, it returns the JSON representation.



Here are a few examples:



| Value | `as<String>()` |
| --- | --- |
| `"hello world"` | `"hello world"` |
| `true` | `"true"` |
| `false` | `"false"` |
| `42` | `"42"` |
| `[1,2,3]` | `"[1,2,3]"` |
| `{"key":"value"}` | `"{\"key\":\"value\"}"` |
| `null` | `"null"` |



If you don’t want this behavior, use `as<const char*>()` instead.



## Integer overflows



`JsonVariantConst::as<T>()` is aware of integer overflows and only returns a value if it can fit in the specified type.



For example, if the value contains `512`, `as<char>()` returns `0`, but `as<int>()` returns `512`.



## User-defined types



`JsonVariantConst::as<T>()` supports user-defined types by calling `convertFromJson()`. For example, to support [`tm`](https://en.cppreference.com/w/c/chrono/tm), you must define the following function:



```cpp
void convertFromJson(JsonVariantConst src, tm& dst) {
  strptime(src.as<const char*>(), "%FT%TZ", &dst);
}
```



For more information about custom converters, please read [the article dedicated to ArduinoJson 6.18.0](https://arduinojson.org/news/2021/05/04/version-6-18-0/).



## Example



```cpp
JsonDocument doc;
deserializeJson(doc, "{\"answer\":42}");

JsonVariantConst answer = doc["answer"];
int i = answer.as<int>(); // <- i == 42
double d = answer.as<double>(); // <- d == 42.0
const char* s = answer.as<const char*>(); // <- s == NULL
```



## See also


* [`JsonVariant::as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/)


---


# JsonVariant::is<T>()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/is/

## Description



`JsonVariant::is<T>()` tests if the variant is currently holding a value of type `T`.



If you want to test that the value exists regardless of its type, you can use `is<JsonVariant>()`.



## Signatures



```cpp
bool is<bool>() const;

bool is<float>() const;
bool is<double>() const;

bool is<signed char>() const;
bool is<unsigned char>() const;
bool is<signed int>() const;
bool is<unsigned int>() const;
bool is<signed short>() const;
bool is<unsigned short>() const;
bool is<signed long>() const;
bool is<unsigned long>() const;
bool is<unsigned long long>() const;   // ⚠️ may require ARDUINOJSON_USE_LONG_LONG
bool is<signed long long>() const;     // ⚠️ may require ARDUINOJSON_USE_LONG_LONG

bool is<const char*>() const;

bool is<JsonArray>() const;
bool is<JsonObject>() const;
bool is<JsonVariant>() const;

bool is<JsonArrayConst>() const;
bool is<JsonObjectConst>() const;
bool is<JsonVariantConst>() const;

bool is<TEnum>() const; // alias of as<int>()
bool is<T>() const;     // calls user-defined function
```



## Return value


* `true` if the variant is currently holding a value of type `T`,
* `false` if not



## JSON types vs. C++ types



Different C++ types can store the same JSON value, so `is<T>()` can return `true` for several `T`s. For example, `is<float>()` always returns the same value as `is<double>()` .



The table below gives the correspondence between the JSON type and the C++ types:



| JSON type | `T` |
| --- | --- |
| Floating point | `float`, `double` |
| Integer | `int`, `short`, `long`, `long long` |
| String | `const char*`, `char*` |
| Boolean | `bool` |
| Array | [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) |
| Object | [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) |



Caution: `is<float>()` and `is<double>()` return `true` for integers too.



## Integer overflows



`JsonVariant::is<T>()` is aware of integer overflows and only returns `true` if the specified type can store the value.



For example if the variant contains `512`, `is<char>()` returns `false`, but `is<int>()` return `true`.



## User-defined types



`JsonVariant::is<T>()` supports user-defined types by calling `canConvertFromJson()`. For example, to support an hypothetical `Complex` class, we could write:



```cpp
bool canConvertFromJson(JsonVariantConst src, const Complex&) {
  return src["real"].is<double>() && src["imag"].is<double>();
}
```



The second parameter of `canConvertFromJson()` is required to trigger [ADL](https://en.cppreference.com/w/cpp/language/adl) but must not be used by the function.



## Example



```cpp
variant.set(42);
variant.is<int>();          // true
variant.is<double>();       // true
variant.is<const char*>();  // false
variant.is<JsonArray>();    // false
```



## See also


* [`JsonVariant::as<T>()`](https://arduinojson.org/v7/api/jsonvariant/as/)
* [`JsonVariant::isNull()`](https://arduinojson.org/v7/api/jsonvariant/isnull/)
* [`JsonVariant::operator|`](https://arduinojson.org/v7/api/jsonvariant/or/)
* [`JsonVariantConst::is<T>()`](https://arduinojson.org/v7/api/jsonvariantconst/is/)


---


# JsonArrayConst
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarrayconst/

## Description



[`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/) is a read-only version of [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/). It’s also twice smaller because it doesn’t contain a pointer to the memory pool.



## Example



```cpp
// parse a JSON array
JsonDocument doc;
deserializeJson(doc, "[1,2,3]");

// extract the values
JsonArrayConst array = doc.as<JsonArray>();
for(JsonVariantConst v : array) {
    Serial.println(v.as<int>());
}
```



## Member functions


* [`begin() / end()`](https://arduinojson.org/v7/api/jsonarrayconst/begin_end/)
* [`isNull()`](https://arduinojson.org/v7/api/jsonarrayconst/isnull/)
* [`nesting()`](https://arduinojson.org/v7/api/jsonarrayconst/nesting/)
* [`operator[]`](https://arduinojson.org/v7/api/jsonarrayconst/subscript/)
* [`size()`](https://arduinojson.org/v7/api/jsonarrayconst/size/)


---


# JsonArray::add()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarray/add/

## Description



`JsonArray::add()` appends a value to the array pointed by the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



If the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is null/unbound, this function does nothing.



## Signatures



```cpp
bool add(bool value) const;

bool add(float value) const;
bool add(double value) const;

bool add(signed char value) const;
bool add(signed long value) const;
bool add(signed int value) const;
bool add(signed short value) const;
bool add(unsigned char value) const;
bool add(unsigned long value) const;
bool add(unsigned int value) const;
bool add(unsigned short value) const;

bool add(const char (&value)[N]) const;           // stores a pointer
bool add(const char *value) const;                // stores a copy
bool add(const __FlashStringHelper *value) const; // stores a copy

bool add(const String &value) const;       // stores a copy
bool add(const std::string &value) const;  // stores a copy
bool add(const Printable& value);          // stores a copy
bool add(std::string_view value);          // stores a copy

bool add(JsonArray array) const;          // stores a deep copy
bool add(JsonObject object) const;        // stores a deep copy
bool add(JsonVariant variant) const;      // stores a deep copy
bool add(const JsonDocument& doc) const;  // stores a deep copy

bool add(TEnum value) const;  // alias of add(int)
bool add(T value) const;      // calls user-defined function

JsonArray   add<JsonArray>() const;    // adds a new empty array
JsonObject  add<JsonObject>() const;   // adds a new empty object
JsonVariant add<JsonVariant>() const;  // adds a new null variant
```



## Arguments



`value`: the value to add to the array.



All values are stored by copy, except string literals which are stored by address.



The string copy policy has changed in [ArduinoJson 7.3](https://arduinojson.org/news/2024/12/29/arduinojson-7-3/).



## Return value



`JsonArray::add()` return a `bool` that tells whether the operation was successful or not:


* `true` if the value was successfully added.
* `false` if there was not enough memory in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## User-defined types



`JsonArray::add()` supports user-defined types by calling `convertToJson()`. For example, to support [`tm`](https://en.cppreference.com/w/c/chrono/tm), you must define:



```cpp
bool convertToJson(const tm& src, JsonVariant dst) {
  char buf[32];
  strftime(buf, sizeof(buf), "%FT%TZ", &src);
  return dst.set(buf);
}
```



## Example



```cpp
JsonDocument doc;
JsonArray array = doc.to<JsonArray>();
array.add("hello");
array.add(3.14156);
serializeJson(doc, Serial);
```



will write



```cpp
["hello",3.14156]
```



## See also


* [`JsonArray::operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/)
* [`copyArray()`](https://arduinojson.org/v7/api/misc/copyarray/)


---


# JsonArray::begin() / JsonArray::end()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarray/begin_end/

## Description



The member functions `begin()` and `end()` return STL-style iterators. You can use these iterators to enumerate all the elements in the array pointed by the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



These functions reproduce the containers in the C++ Standard Library and allow you to use the “ranged-based for loop” feature of C++11. See the example below.



## Signatures



```cpp
JsonArray::iterator begin() const;
JsonArray::iterator end() const;
```



## Return value



`begin()` returns an iterator to the first element of the array.



`end()` returns an iterator to the element after the last. This iterator must not be dereferenced because it is out of the array. It’s a placeholder to detect the end of the array.



`JsonArray::iterator` points to a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/). You can dereference the iterator with the usual `*` and `->` operators.



## Example



```cpp
JsonDocument doc;
deserializeJson(doc, "[\"one\",\"two\",\"three\"]");
JsonArray arr = doc.as<JsonArray>();

for (JsonVariant value : arr) {
    Serial.println(value.as<const char*>());
}
```



The code above would print:



```cpp
one
two
three
```



## See also


* [`JsonObject::begin()` / `JsonObject::end()`](https://arduinojson.org/v7/api/jsonobject/begin_end/)
* To recursively walk the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), see the “Recursive Analyzer” case study in [Mastering ArduinoJson](https://arduinojson.org/book/)


---


# JsonArray::clear()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarray/clear/

## Description



[`JsonArray::clear()`](https://arduinojson.org/v7/api/jsonarray/clear/) removes all elements from the array pointed by the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



If the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is null/unbound, this function does nothing.



## Signatures



```cpp
void clear() const;
```



## See also


* [`JsonDocument::clear()`](https://arduinojson.org/v7/api/jsondocument/clear/)


---


# JsonArray::isNull()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarray/isnull/

## Description



[`JsonArray::isNull()`](https://arduinojson.org/v7/api/jsonarray/isnull/) tells whether the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) points to an array or not.



You can use this function to:


1. check if the array was successfully parsed, or
2. check if the array was successfully allocated.



As an alternative, you can use the conversion to `bool`; for example, `if(array)` instead of `if(!array.isNull())`



## Signature



```cpp
bool isNull() const;
```



## Return value



[`JsonArray::isNull()`](https://arduinojson.org/v7/api/jsonarray/isnull/) returns a `bool` that tells if the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) points to something:


* `true` if the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is null,
* `false` if the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is valid and points to an array.



## Examples



Example 1: parsing success:



```cpp
JsonDocument doc;
deserializeJson(doc, "[1,2]");
JsonArray array = doc.as<JsonArray>();
Serial.println(array.isNull()); // false
```



Example 2: parsing failure:



```cpp
JsonDocument doc;
deserializeJson(doc, "{1,2}");
JsonArray array = doc.as<JsonArray>();
Serial.println(array.isNull()); // true
```



Example 3: allocation success:



```cpp
JsonDocument doc;
JsonArray array = doc.to<JsonArray>();
Serial.println(array.isNull()); // false
```



Example 4: allocation failure:



```cpp
JsonDocument doc;
JsonArray array = doc.to<JsonArray>();
Serial.println(array.isNull()); // true
```



## See also


* [`JsonObject::isNull()`](https://arduinojson.org/v7/api/jsonobject/isnull/)


---


# JsonArray::nesting()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarray/nesting/

## Description



`JsonArray::nesting()` returns the depth (i.e., the nesting level) of the array.



| JSON | Nesting |
| --- | --- |
| `null` | 0 |
| `[0]` | 1 |
| `[[0]]` | 2 |
| `[{"a":0}]` | 2 |
| `[{"a":[0]}]` | 3 |



## Signature



```cpp
size_t nesting() const;
```



## See also


* [`JsonDocument::nesting()`](https://arduinojson.org/v7/api/jsondocument/nesting/)


---


# JsonArray::operator[]
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarray/subscript/

## Description



[`JsonArray::operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/) gets or replaces a value in the array pointed by the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



If the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is null, this operator does nothing.



If you need to know if the insertion succeed, use [`JsonVariant::set()`](https://arduinojson.org/v7/api/jsonvariant/set/) instead.



Don’t use this operator to get all values; instead, use [iterators](https://arduinojson.org/v7/api/jsonarray/begin_end/).



## Signatures



```cpp
ElementProxy operator[](size_t index) const;
```



## Argument



`index`: the zero-based position of the value in the array.



## Return value



[`JsonArray::operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/) returns a proxy class that allows you to use the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) as a C array.



If this concept of proxy class is unfamiliar to you, just think of a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) instead.



The proxy class is non-copyable, so you cannot store it in a variable; you must use it immediately. This implies that you cannot use the `auto` keyword to store the result of this operator; instead, you must specify the type explicitly as show in the example below.



## Example



```cpp
JsonArray array = doc.to<JsonArray>();
array.add(42);
int value = array[0];
array[0] = 666;
```



If you want to use the `auto` keyword, you must specify the type explicitly:



```cpp
auto value = array[0].as<int>();
```



## See also


* [`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/)
* [`JsonVariant::operator[]`](https://arduinojson.org/v7/api/jsonvariant/subscript/)


---


# JsonArray::remove()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarray/remove/

## Description



`JsonArray::remove()` removes the element at the specified index from the array pointed by the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



If the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is null/unbound, this function does nothing.



## Signature



```cpp
void remove(size_t index) const;

void remove(JsonArray::iterator iterator) const;
```



## Arguments



`index`: the zero-based position of the element in the array.



`iterator`: an iterator returned by [`JsonArray::begin()`](https://arduinojson.org/v7/api/jsonarray/begin_end/).



## Examples



In the following examples, we will use this JSON document as input:



```cpp
{
  "survivors": [
    {
      "name": "Coach",
      "age": 40,
      "sex": "male",
      "occupation": "teacher"
    },
    {
      "name": "Ellis",
      "age": 23,
      "sex": "male",
      "occupation": "mechanic"
    },
    {
      "name": "Nick",
      "age": 35,
      "sex": "male",
      "occupation": "retailer"
    },
    {
      "name": "Rochelle",
      "age": 29,
      "sex": "female",
      "occupation": "associate producer"
    }
  ]
}
```



### Example 1: Remove the element at specified index



If you want to remove the second member of the crew, you can simply use its index (`1`, since the indexing starts at zero):



```cpp
deserializeJson(doc, input);
JsonArray crew = doc["survivors"];
crew.remove(1);
serializeJsonPretty(object, Serial);
```



Execute this program and it will print the following to the serial:



```cpp
{
  "survivors": [
    {
      "name": "Coach",
      "age": 40,
      "sex": "male",
      "occupation": "teacher"
    },
    {
      "name": "Nick",
      "age": 35,
      "sex": "male",
      "occupation": "retailer"
    },
    {
      "name": "Rochelle",
      "age": 29,
      "sex": "female",
      "occupation": "associate producer"
    }
  ]
}
```



### Example 2: Remove all elements matching a criteria



Here is how you can remove all female characters from the crew:



```cpp
deserializeJson(doc, input);
JsonArray crew = doc["survivors"];

for (JsonArray::iterator it=crew.begin(); it!=crew.end(); ++it) {
  if ((*it)["sex"] == "female") {
     crew.remove(it);
  }
}

serializeJsonPretty(object, Serial);
```



Run this program and it will write theses lines to the serial:



```cpp
{
  "survivors": [
    {
      "name": "Coach",
      "age": 40,
      "sex": "male",
      "occupation": "teacher"
    },
    {
      "name": "Ellis",
      "age": 23,
      "sex": "male",
      "occupation": "mechanic"
    },
    {
      "name": "Nick",
      "age": 35,
      "sex": "male",
      "occupation": "retailer"
    }
  ]
}
```



## See also


* [`JsonObject::remove()`](https://arduinojson.org/v7/api/jsonobject/remove/)


---


# JsonArray::set()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarray/set/

## Description



[`JsonArray::set()`](https://arduinojson.org/v7/api/jsonarray/set/) replaces the elements of the array pointed by the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) with values from another array.



If the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is null/unbound, this function does nothing.



## Signatures



```cpp
bool set(JsonArrayConst src) const;
```



## Arguments



`src`: a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) that points to the array to copy.



## Return value



[`JsonArray::set()`](https://arduinojson.org/v7/api/jsonarray/set/) returns a `bool` that tells whether the operation was successful or not:


* `true` if the operation is successful
* `false` if there was not enough room in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## See also


* [JsonObject::set()](https://arduinojson.org/v7/api/jsonobject/set/)
* [JsonVariant::set()](https://arduinojson.org/v7/api/jsonvariant/set/)


---


# JsonArray::size()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarray/size/

## Description



[`JsonArray::size()`](https://arduinojson.org/v7/api/jsonarray/size/) gets the number of elements in the array pointed by the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



If the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) is null, this function returns `0`.



Internally, this function walks a linked-list to count the elements, so its [time complexity](https://en.wikipedia.org/wiki/Time_complexity) is *O(n)*.



Don’t use this function to create a `for` loop; instead, use [iterators](https://arduinojson.org/v7/api/jsonarray/begin_end/).



## Signature



```cpp
size_t size() const;
```



## Return value



[`JsonArray::size()`](https://arduinojson.org/v7/api/jsonarray/size/) returns an unsigned integer containing the number of elements in the array.



## Example



```cpp
JsonArray array = doc.to<JsonArray>();
array.add("hello");
array.add("world");
Serial.println(array.size()); // 2
```



## See also


* [`JsonObject::size()`](https://arduinojson.org/v7/api/jsonobject/size/)


---


# JsonVariant::clear()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/clear/

## Description



[`JsonVariant::clear()`](https://arduinojson.org/v7/api/jsonvariant/clear/) sets the variant pointed by the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) to null.



## Signatures



```cpp
void clear() const;
```



## See also


* [`JsonDocument::clear()`](https://arduinojson.org/v7/api/jsondocument/clear/)


---


# JsonVariant::containsKey()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/containskey/

> > ## Deprecated



> `containsKey()` is deprecated since ArduinoJson 7.2 and you must use [`is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/) instead.



> ```cpp
// Before
if (doc.containsKey("value")) {
  int value = doc["value"];
  // ...
}

// After
if (doc["value"].is<int>()) {
  int value = doc["value"];
  // ...
}
> ```



> If you want to check that a key exists regardless of its type, you can use `is<JsonVariant>()`.



## Description



[`JsonVariant::containsKey()`](https://arduinojson.org/v7/api/jsonvariant/containskey/) tests whether a key exists in the object pointed by the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/).



This function only works if the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) points to an object. It returns `false` if the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is null, or if it points to an array.



This function can (and should) be avoided most of the time. See below.



## Signature



```cpp
bool containsKey(const char* key) const;
bool containsKey(const String& key) const;
bool containsKey(const std::string& key) const;
bool containsKey(const __FlashStringHelper& key) const;
bool containsKey(std::string_view key) const;
```



## Arguments



`key`: the key to look for.



## Return value



[`JsonVariant::containsKey()`](https://arduinojson.org/v7/api/jsonvariant/containskey/) returns a `bool` that tells whether the key was found or not:


* `true` if the key is present in the object
* `false` if the key is absent of the object



## Example



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

JsonVariant location = doc["location"];
bool hasCity = location.containsKey("city"); // true
bool hasCountry = location.containsKey("country"); // false
```



## Avoid this function when you can!



This function can (and should) be avoided most of the time.



Because ArduinoJson implements the [Null Object Pattern](https://en.wikipedia.org/wiki/Null_Object_pattern), it is always safe to read the object: if the key doesn’t exist, it returns an empty value. For example:



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

JsonVariant location = doc["location"];
if (location.containsKey("city")) {
  const char* city = location["city"];
  Serial.println(city);
}
```



Can be written like this:



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

const char* city = doc["location"]["city"];
if (city)
  Serial.println(city);
```



This version should lead to a **smaller** and **faster** code since it only does the lookup once.



## How to test nested keys?



You cannot test the presence of nested keys with `containsKey()` but, as explained above, it’s safe to read the object anyway.



For example, when [Weather Underground](http://www.wunderground.com) returns an error like:



```cpp
{
  "response": {
    "version": "0.1",
    "termsofService": "http://www.wunderground.com/weather/api/d/terms.html",
    "features": {
      "conditions": 1
    },
    "error": {
      "type": "querynotfound",
      "description": "No cities match your search query"
    }
  }
}
```



You should not try to call `containsKey("response")`, `containsKey("error")` and `containsKey("description")`. Instead, just get the value and test if it’s null:



```cpp
const char* error = doc["response"]["error"]["description"];
if (error) {
  Serial.println(error);
  return;
}
```



## See also


* [`JsonVariantConst::containsKey()`](https://arduinojson.org/v7/api/jsonvariantconst/containskey/)


---


# JsonVariant::isNull()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/isnull/

## Description



`JsonVariant::isNull()` tests if the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is unbound or points to a `null` value.



Remember that a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is a reference to a value in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). A [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) that is not attached to a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) will always be null:



```cpp
JsonVariant variant;
variant.set(42); // does nothing because the variant is not attached to a document
variant.isNull(); // returns true
```



As an alternative, you can use the conversion to `bool`; for example, `if(variant)` instead of `if(!variant.isNull())`



## Signature



```cpp
bool isNull() const;
```



## Return value


* `true` if the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is unbound or points to a `null`.
* `false` if the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) points to a non-null value.



## Example



```cpp
JsonDocument doc;
deserializeJson("{\"a\":1,\"b\":null}");

doc["a"].isNull(); // false, because doc["a"] contains 1
doc["b"].isNull(); // true, because doc["b"] contains null
doc["c"].isNull(); // true, because doc["c"] doesn't exist
```



## See also


* [`JsonVariant::is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/)
* [`JsonVariant::operator|`](https://arduinojson.org/v7/api/jsonvariant/or/)
* [`JsonVariantConst::isNull()`](https://arduinojson.org/v7/api/jsonvariantconst/isnull/)


---


# JsonVariant::nesting()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/nesting/

## Description



`JsonVariant::nesting()` returns the depth (i.e., the nesting level) of the value pointed by the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/)



| JSON | Nesting |
| --- | --- |
| `null` | 0 |
| `"string"` | 0 |
| `[0]` | 1 |
| `[[0]]` | 2 |
| `{"a":0}` | 1 |
| `{"a":{"b":0}}` | 2 |
| `{"a":{"b":[0]}}` | 3 |



## Signature



```cpp
size_t nesting() const;
```



## See also


* [`JsonDocument::nesting()`](https://arduinojson.org/v7/api/jsondocument/nesting/)


---


# JsonVariant::operator[]
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/subscript/

## Description



[`JsonVariant::operator[]`](https://arduinojson.org/v7/api/jsonvariant/subscript/) gets, replaces, adds a value in the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/).



Depending on the argument type the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) can be used like a [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) or a [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



All values are stored by copy, except string literals which are stored by address.



The string copy policy has changed in [ArduinoJson 7.3](https://arduinojson.org/news/2024/12/29/arduinojson-7-3/).



## Signatures



```cpp
// mimics a JsonArray
ElementProxy operator[](size_t index) const;

// mimics a JsonObject
MemberProxy operator[](const char (&key)[N]) const;
MemberProxy operator[](const char* key) const;
MemberProxy operator[](const String& key) const;
MemberProxy operator[](const std::string& key) const;
MemberProxy operator[](string_view key) const;
MemberProxy operator[](const __FlashStringHelper* key) const;
```



## Arguments



`index`: the index in the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



`key`: the key in the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/)



## Return value



A proxy class that allows using the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) as an array or a dictionary. If this concept of proxy class is unfamiliar to you, just think of a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) instead.



Please see [`JsonArray::operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/) and [`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/) for explanations about `ElementProxy` and `MemberProxy`.



## See also


* [`JsonArray::operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/)
* [`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/)
* [`JsonVariantConst::operator[]`](https://arduinojson.org/v7/api/jsonvariantconst/subscript/)


---


# JsonVariant::remove()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/remove/

## Description



Depending on the type of its argument, `JsonVariant::remove()` either:


* removes the element at the specified index from the array pointed by the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/)
* removes the member with the specified key in the object pointed by the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/)



In other words, `JsonVariant::remove()` behaves like [`JsonArray::remove()`](https://arduinojson.org/v7/api/jsonarray/remove/) or [`JsonObject::remove()`](https://arduinojson.org/v7/api/jsonobject/remove/).



## Signature



```cpp
void remove(size_t index) const;

void remove(const char* key) const;
void remove(const __FlashStringHelper* key) const;
void remove(const String& key) const;
void remove(const std::string& key) const;
void remove(std::string_view key) const;
```



## Arguments



`index`: the zero-based position of the array element to remove.



`key`: the key of the object member to remove



## See also


* [`JsonArray::remove()`](https://arduinojson.org/v7/api/jsonarray/remove/)
* [`JsonObject::remove()`](https://arduinojson.org/v7/api/jsonobject/remove/)


---


# JsonVariant::set()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/set/

## Description



`JsonVariant::set()` replaces the value stored in the variant. The variant may change its type if necessary.



Instead of this function, use [`JsonArray::operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/) or [`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/) because they offer a more intuitive syntax



## Signatures



```cpp
bool set(bool value) const;

bool set(float value) const;
bool set(double value) const;

bool set(signed char value) const;
bool set(signed long value) const;
bool set(signed int value) const;
bool set(signed short value) const;
bool set(unsigned char value) const;
bool set(unsigned long value) const;
bool set(unsigned int value) const;
bool set(unsigned short value) const;

bool set(const char (&value)[N]) const;            // stores a pointer
bool set(const char *value) const;                 // stores a copy
bool set(const __FlashStringHelper *value) const;  // stores a copy

bool set(const String &value) const;       // stores a copy
bool set(const std::string &value) const;  // stores a copy
bool set(const Printable& value) const;    // stores a copy
bool set(std::string_view value) const;    // stores a copy

bool set(JsonArray array) const;          // stores a deep copy
bool set(JsonObject object) const;        // stores a deep copy
bool set(JsonVariant variant) const;      // stores a deep copy
bool set(const JsonDocument& doc) const;  // stores a deep copy

bool set(TEnum value) const;  // alias of set(int)
bool set(T value) const;      // calls user-defined function
```



## Arguments



`value`: the new value of the variant, it can be any type supported by ArduinoJson or a user-defined type if you define [custom converters](https://arduinojson.org/news/2021/05/04/version-6-18-0/).



All values are stored by copy, except string literals which are stored by address.



The string copy policy has changed in [ArduinoJson 7.3](https://arduinojson.org/news/2024/12/29/arduinojson-7-3/).



## Return value



`JsonVariant::set()` returns a `bool` that tells whether the operation was successful or not:


* `true` if the value operation was successful.
* `false` if there was not enough room in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## User-defined types



`JsonVariant::set()` supports user-defined types by calling `convertToJson()`. For example, to support [`tm`](https://en.cppreference.com/w/c/chrono/tm), you must define:



```cpp
bool convertToJson(const tm& src, JsonVariant dst) {
  char buf[32];
  strftime(buf, sizeof(buf), "%FT%TZ", &src);
  return dst.set(buf);
}
```



## Example



```cpp
JsonDocument doc;
bool success = doc["hello"].set("world");
serializeJson(doc, Serial);
```



The above program produces the following output:



```cpp
{"hello":"world"}
```



## See also


* [`JsonArray::operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/)
* [`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/)


---


# JsonVariant::size()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariant/size/

## Description



If the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) points to an object, `JsonVariant::size()` returns the number of key-value pairs in the object (same as [`JsonObject::size()`](https://arduinojson.org/v7/api/jsonobject/size/))



If the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) points to an array, `JsonVariant::size()` returns the number of elements in the array (same as [`JsonArray::size()`](https://arduinojson.org/v7/api/jsonarray/size/))



In all other cases, `JsonVariant::size()` returns `0`



Internally, this function walks a linked-list to count the elements, so its [time complexity](https://en.wikipedia.org/wiki/Time_complexity) is *O(n)*.



## Signature



```cpp
size_t size() const;
```



## See also


* [`JsonArray::size()`](https://arduinojson.org/v7/api/jsonarray/size/)
* [`JsonObject::size()`](https://arduinojson.org/v7/api/jsonobject/size/)
* [`JsonDocument::size()`](https://arduinojson.org/v7/api/jsondocument/size/)


---


# JsonObjectConst
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobjectconst/

## Description



[`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/) is a read-only version of [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/). It’s also twice smaller because it doesn’t contain a pointer to the memory pool.



## Example



```cpp
JsonDocument doc;

// deserialize the object
char json[] = "{\"hello\":\"world\"}";
deserializeJson(doc, json);

// extract the data
JsonObjectConst object = doc.as<JsonObject>();
const char* world = object["hello"];
```



## Member functions


* [`begin() / end()`](https://arduinojson.org/v7/api/jsonobjectconst/begin_end/)
* [`containsKey()`](https://arduinojson.org/v7/api/jsonobjectconst/containskey/) (deprecated)
* [`isNull()`](https://arduinojson.org/v7/api/jsonobjectconst/isnull/)
* [`nesting()`](https://arduinojson.org/v7/api/jsonobjectconst/nesting/)
* [`operator[]`](https://arduinojson.org/v7/api/jsonobjectconst/subscript/)
* [`size()`](https://arduinojson.org/v7/api/jsonobjectconst/size/)



## See also


* [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/)
* [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/)


---


# JsonObject::begin() / JsonObject::end()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobject/begin_end/

## Description



The member functions `begin()` and `end()` return STL-style iterators. You can use these iterators to enumerate all the key-value pairs in the object pointed by the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



These functions reproduce the containers in the C++ Standard Library and allow you to use the “ranged-based for loop” feature of C++11. See the example below.



## Signatures



```cpp
JsonObject::iterator begin() const;
JsonObject::iterator end() const;
```



## Return value



`begin()` returns an iterator to the first key-value pair of the object.



`end()` returns an iterator to the element after the last. This iterator must not be dereferenced; it’s a placeholder to detect the end of the object.



`JsonObject::iterator` points to a `JsonPair`, a class that bundles a key (accessible via `JsonPair::key()`) and a value (accessible via `JsonPair::value()`).



The type of the key is [`JsonString`](https://arduinojson.org/v7/api/jsonstring/); you need to call `JsonString::c_str()` to get the char-pointer out of it. The type of the value is [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/), as usual.



Here is a summary:



```cpp
class JsonPair {
public:
  JsonString key() const;
  JsonVariant value() const;
};

class JsonString {
public:
  const char* c_str() const;
};
```



## Example



### List all keys and values of an object



```cpp
JsonDocument doc;
deserializeJson(doc, "{\"first\":\"hello\",\"second\":\"world\"}");
JsonObject root = doc.as<JsonObject>();

for (JsonPair kv : root) {
    Serial.println(kv.key().c_str());
    Serial.println(kv.value().as<const char*>());
}
```



The code above prints:



```cpp
first
hello
second
world
```



### Get object member by index



```cpp
JsonDocument doc;
deserializeJson(doc, "{\"a\":\"alpha\",\"b\":\"beta\",\"c\":\"charlie\"}");

JsonObject::iterator it = doc.as<JsonObject>().begin();

int index = 2;
for (int i = 0; i < index; i++)
    ++it;

Serial.println(it->value().as<const char*>());  // prints "charlie"
```



## See also


* [`JsonArray::begin()` / `JsonArray::end()`](https://arduinojson.org/v7/api/jsonarray/begin_end/)
* To recursively walk the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), see the “Recursive Analyzer” case study in [Mastering ArduinoJson](https://arduinojson.org/book/)


---


# JsonObject::clear()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobject/clear/

## Description



[`JsonObject::clear()`](https://arduinojson.org/v7/api/jsonobject/clear/) removes all members from the object pointed by the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



If the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) is null/unbound, this function does nothing.



## Signatures



```cpp
void clear() const;
```



## See also


* [`JsonDocument::clear()`](https://arduinojson.org/v7/api/jsondocument/clear/)


---


# JsonObject::containsKey()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobject/containskey/

> > ## Deprecated



> `containsKey()` is deprecated since ArduinoJson 7.2 and you must use [`is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/) instead.



> ```cpp
// Before
if (doc.containsKey("value")) {
  int value = doc["value"];
  // ...
}

// After
if (doc["value"].is<int>()) {
  int value = doc["value"];
  // ...
}
> ```



> If you want to check that a key exists regardless of its type, you can use `is<JsonVariant>()`.



## Description



`JsonObject::containsKey()` tests whether a key exists in the object pointed by the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



If the [`JsonObject`](https://arduinojson.org/v5/api/jsonobject/) is null, this function returns `false`.



This function can (and should) be avoided most of the time. See below.



## Signature



```cpp
bool containsKey(const char* key) const;
bool containsKey(const String& key) const;
bool containsKey(const std::string& key) const;
bool containsKey(const __FlashStringHelper& key) const;
bool containsKey(std::string_view key) const;
```



## Arguments



`key`: the key to look for.



## Return value



`JsonObject::containsKey()` returns a `bool` that tells whether the key was found or not:


* `true` if the key is present in the object
* `false` if the key is absent of the object



## Example



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

JsonObject location = doc["location"];
bool hasCity = location.containsKey("city"); // true
bool hasCountry = location.containsKey("country"); // false
```



## Avoid this function when you can!



This function can (and should) be avoided most of the time.



Because ArduinoJson implements the [Null Object Pattern](https://en.wikipedia.org/wiki/Null_Object_pattern), it is always safe to read the object: if the key doesn’t exist, it returns an empty value. For example:



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

JsonObject location = doc["location"];
if (location.containsKey("city")) {
  const char* city = location["city"];
  Serial.println(city);
}
```



Can be written like this:



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

const char* city = doc["location"]["city"];
if (city)
  Serial.println(city);
```



This version should lead to a **smaller** and **faster** code since it only does the lookup once.



## How to test nested keys?



You cannot test the presence of nested keys with `containsKey()` but, as explained above, it’s safe to read the object anyway.



For example, when [Weather Underground](http://www.wunderground.com) returns an error like:



```cpp
{
  "response": {
    "version": "0.1",
    "termsofService": "http://www.wunderground.com/weather/api/d/terms.html",
    "features": {
      "conditions": 1
    },
    "error": {
      "type": "querynotfound",
      "description": "No cities match your search query"
    }
  }
}
```



You should not try to call `containsKey("response")`, `containsKey("error")` and `containsKey("description")`. Instead, just get the value and test if it’s null:



```cpp
const char* error = doc["response"]["error"]["description"];
if (error) {
  Serial.println(error);
  return;
}
```



## See also


* [`JsonObjectConst::containsKey()`](https://arduinojson.org/v7/api/jsonobjectconst/containskey/)


---


# JsonObject::isNull()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobject/isnull/

## Description



[`JsonObject::isNull()`](https://arduinojson.org/v7/api/jsonobject/isnull/) tests whether the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) points to an object or not.



You can use this function to:


1. check if the object was successfully parsed, or
2. check if the object was successfully allocated.



As an alternative, you can use the conversion to `bool`; for example, `if(object)` instead of `if(!object.isNull())`



## Signature



```cpp
bool isNull() const;
```



## Return value



[`JsonObject::isNull()`](https://arduinojson.org/v7/api/jsonobject/isnull/) returns a `bool` that tells if the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) points to something:


* `true` if the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) is null,
* `false` if the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) is valid and points to an object.



## Examples



Example 1: parsing success:



```cpp
JsonDocument doc;
deserializeJson(doc, "{\"hello\":\"world\"}");
JsonObject object = doc.as<JsonObject>();
Serial.println(object.isNull()); // false
```



Example 2: parsing failure:



```cpp
JsonDocument doc;
deserializeJson(doc, "[\"hello\",\"world\"]");
JsonObject object = doc.as<JsonObject>();
Serial.println(object.isNull()); // true
```



Example 3: allocation success:



```cpp
JsonDocument doc;
JsonObject object = doc.to<JsonObject>();
Serial.println(object.isNull()); // false
```



Example 4: allocation failure:



```cpp
JsonDocument doc;
JsonObject object = doc.to<JsonObject>();
Serial.println(object.isNull()); // true
```



## See also


* [`JsonArray::isNull()`](https://arduinojson.org/v7/api/jsonarray/isnull/)


---


# JsonObject::nesting()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobject/nesting/

## Description



`JsonObject::nesting()` returns the depth (i.e., the nesting level) of the object pointed by the [`JsonObject`](https://arduinojson.org/v7/api/jsonvariant/)



| JSON | Nesting |
| --- | --- |
| `null` | 0 |
| `{"a":0}` | 1 |
| `{"a":{"b":0}}` | 2 |
| `{"a":{"b":[0]}}` | 3 |



## Signature



```cpp
size_t nesting() const;
```



## See also


* [`JsonDocument::nesting()`](https://arduinojson.org/v7/api/jsondocument/nesting/)


---


# JsonObject::operator[]
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobject/subscript/

## Description



[`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/) gets, replaces, or adds a value in the object pointed by the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



If the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) is null/unbound, this function does nothing.



All values are stored by copy, except string literals which are stored by address.



The string copy policy has changed in [ArduinoJson 7.3](https://arduinojson.org/news/2024/12/29/arduinojson-7-3/).



If you need to know if the insertion succeed, use [`JsonVariant::set()`](https://arduinojson.org/v7/api/jsonvariant/set/) instead.



## Signatures



```cpp
MemberProxy operator[](const char (&value)[N]) const;
MemberProxy operator[](const char* key) const;
MemberProxy operator[](const __FlashStringHelper* key) const;

MemberProxy operator[](const String& key) const;
MemberProxy operator[](const std::string& key) const;
MemberProxy operator[](std::string_view key) const;
```



## Arguments



`key`: the key that the value will be associated with.



## Return value



[`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/) return a proxy class that allows using the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) like a dictionary.



If this concept of proxy class is unfamiliar to you, just think of a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) instead.



The proxy class is non-copyable, so you cannot store it in a variable; you must use it immediately. This implies that you cannot use the `auto` keyword to store the result of this operator; instead, you must specify the type explicitly as show in the example below.



## Example



```cpp
JsonDocument doc;
JsonObject object = doc.to<JsonObject>();
object["hello"] = "world";
const char* world = object["hello"];
```



If you want to use the `auto` keyword, you must specify the type explicitly:



```cpp
auto world = object["hello"].as<const char*>();
```



## See also


* [`JsonArray::operator[]`](https://arduinojson.org/v7/api/jsonarray/subscript/)
* [`JsonVariant::operator[]`](https://arduinojson.org/v7/api/jsonvariant/subscript/)


---


# JsonObject::remove()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobject/remove/

## Description



`JsonObject::remove()` removes a key-value pair from the object pointed by the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



If the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) is null/unbound, this function does nothing.



## Signatures



```cpp
void remove(const char* key) const;
void remove(const __FlashStringHelper* key) const;

void remove(const String& key) const;
void remove(const std::string& key) const;
void remove(std::string_view key) const;

void remove(JsonObject::iterator iterator) const;
```



## Arguments



`key`: the key to remove from the object.



`iterator`: an iterator returned by [`JsonObject::begin()`](https://arduinojson.org/v7/api/jsonobject/begin_end/)



## Examples



In the following examples, we will use this JSON document as input:



```cpp
{
  "survivors": {
    "Coach": {
      "age": 40,
      "sex": "male",
      "occupation": "teacher"
    },
    "Ellis": {
      "age": 23,
      "sex": "male",
      "occupation": "mechanic"
    },
    "Nick": {
      "age": 35,
      "sex": "male",
      "occupation": "retailer"
    },
    "Rochelle": {
      "age": 29,
      "sex": "female",
      "occupation": "associate producer"
    }
  }
}
```



### Example 1: Remove a member with the specified key



Here is how you can remove “Ellis” from the crew:



```cpp
deserializeJson(doc, input);
JsonObject crew = doc["survivors"];
crew.remove("Ellis");
serializeJsonPretty(object, Serial);
```



Execute this program and it will print the following to the serial:



```cpp
{
  "survivors": {
    "Coach": {
      "age": 40,
      "sex": "male",
      "occupation": "teacher"
    },
    "Nick": {
      "age": 35,
      "sex": "male",
      "occupation": "retailer"
    },
    "Rochelle": {
      "age": 29,
      "sex": "female",
      "occupation": "associate producer"
    }
  }
}
```



### Example 2: Remove all members matching a criteria



Here is how you can remove all female characters from the crew:



```cpp
deserializeJson(doc, input);
JsonObject crew = doc["survivors"];

for (JsonObject::iterator it=crew.begin(); it!=crew.end(); ++it) {
  if (it->value()["sex"] == "female") {
     crew.remove(it);
  }
}

serializeJsonPretty(object, Serial);
```



Run this program and it will write theses lines to the serial:



```cpp
{
  "survivors": {
    "Coach": {
      "age": 40,
      "sex": "male",
      "occupation": "teacher"
    },
    "Ellis": {
      "age": 23,
      "sex": "male",
      "occupation": "mechanic"
    },
    "Nick": {
      "age": 35,
      "sex": "male",
      "occupation": "retailer"
    }
  }
}
```



## See also


* [`JsonArray::remove()`](https://arduinojson.org/v7/api/jsonarray/remove/)


---


# JsonObject::set()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobject/set/

## Description



[`JsonObject::set()`](https://arduinojson.org/v7/api/jsonobject/set/) copies the content of an object to the object pointed by the [`JsonObject`](https://arduinojson.org/v5/api/jsonobject/).



If the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) is null/unbound, this function does nothing.



## Signatures



```cpp
bool set(JsonObjectConst obj) const;
```



## Arguments



`obj`: a [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/) pointing to the object to copy.



## Return value



[`JsonObject::set()`](https://arduinojson.org/v7/api/jsonobject/set/) returns a `bool` that tells whether the operation was successful or not:


* `true` if the operation is successful
* `false` if there was not enough room in the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/).



## See also


* [JsonArray::set()](https://arduinojson.org/v7/api/jsonarray/set/)
* [JsonVariant::set()](https://arduinojson.org/v7/api/jsonvariant/set/)


---


# JsonObject::size()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobject/size/

## Description



[`JsonObject::size()`](https://arduinojson.org/v7/api/jsonobject/size/) returns the number of key-value pairs in the object pointed by the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/).



Internally, this function walks a linked-list to count the elements, so its [time complexity](https://en.wikipedia.org/wiki/Time_complexity) is *O(n)*.



If you want to loop over each key-value pair in the object, use [iterators](https://arduinojson.org/v7/api/jsonobject/begin_end/).



## Signature



```cpp
size_t size() const;
```



## Return value



[`JsonObject::size()`](https://arduinojson.org/v7/api/jsonobject/size/) return an unsigned integer that contains the number of key-value pairs in the object.



## Example



```cpp
JsonObject object = doc.to<JsonObject>();
object["hello"] = "world";
Serial.println(object.size()); // 1
```



## See also


* [`JsonArray::size()`](https://arduinojson.org/v7/api/jsonarray/size/)


---


# JsonVariantConst::operator|
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariantconst/or/

## Description



`JsonVariantConst::operator|()` provides default value for a [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/).



## Signatures



```cpp
bool               operator|(bool defaultValue) const;

float              operator|(float defaultValue) const;
double             operator|(double defaultValue) const;

signed char        operator|(signed char defaultValue) const;
unsigned char      operator|(unsigned char defaultValue) const;
signed int         operator|(signed int defaultValue) const;
unsigned int       operator|(unsigned int defaultValue) const;
signed short       operator|(signed short defaultValue) const;
unsigned short     operator|(unsigned short defaultValue) const;
signed long        operator|(signed long defaultValue) const;
unsigned long      operator|(unsigned long defaultValue) const;
unsigned long long operator|(unsigned long long defaultValue) const; // <- may require ARDUINOJSON_USE_LONG_LONG
signed long long   operator|(signed long long defaultValue) const;   // <- may require ARDUINOJSON_USE_LONG_LONG

const char*        operator|(char* defaultValue) const;
const char*        operator|(const char* defaultValue) const;
String             operator|(String defaultValue) const;
std::string        operator|(const std::string& defaultValue) const;
```



## Arguments



`defaultValue`: the value to return if the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) is null or incompatible with the requested type.



Unlike [`JsonVariantConst::as<T>()`](https://arduinojson.org/v7/api/jsonvariantconst/as/) that uses a template parameter to specify the requested type, `JsonVariantConst::operator|()` infers the requested type from the argument `defaultValue`. Usually, that’s what you want but can lead to odd results as described below.



## Return value


* The value of the variant, if it can be converted to the type of `defaultValue`.
* `defaultValue` if not.



## Example



```cpp
int port = config["port"] | 80;
strlcpy(hostname, config["hostname"] | "example.com", sizeof(hostname));
```



## Pitfall ⚠️



When you use this operator, the type of the expression is inferred from the type of the default value.



Here is an example where this fact matters:



```cpp
long value = root["key"] | -1;
```



In this example, the right side of the expression returns an `int` (and not a `long` as one could expect) because the default value is an integer. If `root["key"]` contains a big value, the integer overflows, and you get a seemingly random result in `value`.



To fix this, you need to change the type of the default value:



```cpp
long value = root["key"] | -1L;
```



Lastly, you cannot combine this operator with [`JsonVariantConst::as<T>()`](https://arduinojson.org/v7/api/jsonvariantconst/as/). For example, you cannot write:



```cpp
// this line compiles but it doesn't do what you think
long value = root["key"].as<long>() | 1; // WRONG!!!!
```



## See also


* [`JsonVariant::operator|()`](https://arduinojson.org/v7/api/jsonvariant/or/)


---


# MessagePack
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/msgpack/

### [`deserializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/deserializemsgpack/)
 Deserialize a MessagePack document into a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/)







### [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/)
 Serializes a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) into a MessagePack document







### [`measureMsgPack()`](https://arduinojson.org/v7/api/msgpack/measuremsgpack/)
 Computes the size of the document that [`serializeMsgPack()`](https://arduinojson.org/v7/api/msgpack/serializemsgpack/) produces







### [`MsgPackBinary`](https://arduinojson.org/v7/api/msgpack/msgpackbinary/) 🆕
 Allows you to read or write MessagePack binary data







### [`MsgPackExtension`](https://arduinojson.org/v7/api/msgpack/msgpackextension/) 🆕
 Allows you to read or write MessagePack extension data


---


# JsonArrayConst::operator[]
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarrayconst/subscript/

## Description



[`JsonArrayConst::operator[]`](https://arduinojson.org/v7/api/jsonarrayconst/subscript/) gets a value in the array pointed by the [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/).



If the [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/) is null, this operator does nothing.



[`JsonArrayConst::operator[]`](https://arduinojson.org/v7/api/jsonarrayconst/subscript/) is a read-only version of [`JsonArray::operator[]()`](https://arduinojson.org/v7/api/jsonarray/subscript/)



## Signatures



```cpp
JsonVariantConst operator[](size_t index) const;
```



## Argument



`index`: the zero-based position of the value in the array.



## Return value



This operator returns a [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/), a read-only reference to the value is the array.



If the index is out of range, this function returns a null reference.



## See also


* [`JsonArray::operator[]()`](https://arduinojson.org/v7/api/jsonarray/subscript/)


---


# JsonObjectConst::operator[]
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobjectconst/subscript/

## Description



[`JsonObjectConst::operator[]`](https://arduinojson.org/v7/api/jsonobjectconst/subscript/) gets a value in the object pointed by the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/).



If the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/) is null/unbound, this function does nothing.



[`JsonObjectConst::operator[]`](https://arduinojson.org/v7/api/jsonobjectconst/subscript/) is a read-only version of [`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/)



## Signatures



```cpp
JsonVariantConst operator[](char* key) const;
JsonVariantConst operator[](const char* key) const;
JsonVariantConst operator[](const __FlashStringHelper* key) const;

JsonVariantConst operator[](const String& key) const;
JsonVariantConst operator[](const std::string& key) const;
JsonVariantConst operator[](std::string_view key) const;
```



## Arguments



`key`: the key associated with the value you want to read.



## Return value



[`JsonObjectConst::operator[]`](https://arduinojson.org/v7/api/jsonobjectconst/subscript/) return a [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/).



## See also


* [`JsonObject::operator[]`](https://arduinojson.org/v7/api/jsonobject/subscript/)


---


# JsonVariantConst::containsKey()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariantconst/containskey/

> > ## Deprecated



> `containsKey()` is deprecated since ArduinoJson 7.2 and you must use [`is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/) instead.



> ```cpp
// Before
if (doc.containsKey("value")) {
  int value = doc["value"];
  // ...
}

// After
if (doc["value"].is<int>()) {
  int value = doc["value"];
  // ...
}
> ```



> If you want to check that a key exists regardless of its type, you can use `is<JsonVariantConst>()`.



## Description



`JsonVariantConst::containsKey()` tests whether a key exists in the object pointed by the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/).



This function only works if the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) points to an object. It returns `false` if the [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is null, or if it points to an array.



This function can (and should) be avoided most of the time. See below.



## Signature



```cpp
bool containsKey(const char* key) const;
bool containsKey(const String& key) const;
bool containsKey(const std::string& key) const;
bool containsKey(const __FlashStringHelper& key) const;
bool containsKey(std::string_view key) const;
```



## Arguments



`key`: the key to look for.



## Return value



`JsonVariantConst::containsKey()` returns a `bool` that tells whether the key was found or not:


* `true` if the key is present in the object
* `false` if the key is absent of the object



## Example



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

JsonVariantConst location = doc["location"];
bool hasCity = location.containsKey("city"); // true
bool hasCountry = location.containsKey("country"); // false
```



## Avoid this function when you can!



This function can (and should) be avoided most of the time.



Because ArduinoJson implements the [Null Object Pattern](https://en.wikipedia.org/wiki/Null_Object_pattern), it is always safe to read the object: if the key doesn’t exist, it returns an empty value. For example:



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

JsonVariantConst location = doc["location"];
if (location.containsKey("city")) {
  const char* city = location["city"];
  Serial.println(city);
}
```



Can be written like this:



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

const char* city = doc["location"]["city"];
if (city)
  Serial.println(city);
```



This version should lead to a **smaller** and **faster** code since it only does the lookup once.



## How to test nested keys?



You cannot test the presence of nested keys with `containsKey()` but, as explained above, it’s safe to read the object anyway.



For example, when [Weather Underground](http://www.wunderground.com) returns an error like:



```cpp
{
  "response": {
    "version": "0.1",
    "termsofService": "http://www.wunderground.com/weather/api/d/terms.html",
    "features": {
      "conditions": 1
    },
    "error": {
      "type": "querynotfound",
      "description": "No cities match your search query"
    }
  }
}
```



You should not try to call `containsKey("response")`, `containsKey("error")` and `containsKey("description")`. Instead, just get the value and test if it’s null:



```cpp
const char* error = doc["response"]["error"]["description"];
if (error) {
  Serial.println(error);
  return;
}
```



## See also


* [`JsonVariant::containsKey()`](https://arduinojson.org/v7/api/jsonvariant/containskey/)


---


# JsonVariantConst::isNull()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariantconst/isnull/

## Description



`JsonVariantConst::isNull()` tests if the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) is unbound or points to a `null` value.



Remember that a [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) is a reference to a value in a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/). A [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) that is not attached to a [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/) will always be null:



```cpp
JsonVariantConst variant;
variant.isNull(); // returns true
```



As an alternative, you can use the conversion to `bool`; for example, `if(variant)` instead of `if(!variant.isNull())`



## Signature



```cpp
bool isNull() const;
```



## Return value


* `true` if the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) is unbound or points to a `null`.
* `false` if the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) is valid and points to a non-null value.



## See also


* [`JsonVariant::isNull()`](https://arduinojson.org/v7/api/jsonvariant/isnull/)


---


# JsonVariantConst::is<T>()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariantconst/is/

## Description



`JsonVariantConst::is<T>()` tests if the variant is currently holding a value of type `T`.



If you want to test that the value exists regardless of its type, you can use `is<JsonVariantConst>()`.



## Signatures



```cpp
bool is<bool>() const;

bool is<float>() const;
bool is<double>() const;

bool is<signed char>() const;
bool is<unsigned char>() const;
bool is<signed int>() const;
bool is<unsigned int>() const;
bool is<signed short>() const;
bool is<unsigned short>() const;
bool is<signed long>() const;
bool is<unsigned long>() const;
bool is<unsigned long long>() const;   // ⚠️ may require ARDUINOJSON_USE_LONG_LONG
bool is<signed long long>() const;     // ⚠️ may require ARDUINOJSON_USE_LONG_LONG

bool is<const char*>() const;

bool is<JsonArray>() const;    // ⚠️ always returns false
bool is<JsonObject>() const;   // ⚠️ always returns false
bool is<JsonVariant>() const;  // ⚠️ always returns false

bool is<JsonArrayConst>() const;
bool is<JsonObjectConst>() const;
bool is<JsonVariantConst>() const;

bool is<TEnum>() const; // alias of as<int>()
bool is<T>() const;     // calls user-defined function
```



## Return value


* `true` if the variant is currently holding a value of type `T`,
* `false` if not



## `JsonArray`, `JsonObject`, and `JsonVariant`



`JsonVariantConst::is<T>()` always returns `false` when `T` is [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), or [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/). This is because [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) is a read-only reference, whereas [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/), [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/), and [`JsonVariant`](https://arduinojson.org/v7/api/jsonvariant/) are read-write references. Indeed, as a read-only reference, [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) cannot be converted to a read-write reference.



You can still test whether the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) points to an array or an object, but for that, you must use [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/) and [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/), which are read-only references to arrays and objects. For example, if you want to test if the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) points to an array, you must do:



```cpp
JsonVariantConst value = doc["key"];
if (value.is<JsonArrayConst>()) {
  // value is an array.
}
```



## JSON types vs. C++ types



Different C++ types can store the same JSON value, so `is<T>()` can return `true` for several `T`s. For example, `is<float>()` always returns the same value as `is<double>()` .



The table below gives the correspondence between the JSON type and the C++ types:



| JSON type | `T` |
| --- | --- |
| Floating point | `float`, `double` |
| Integer | `int`, `short`, `long`, `long long` |
| String | `const char*`, `char*` |
| Boolean | `bool` |
| Array | [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/) |
| Object | [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/) |



Caution: `is<float>()` and `is<double>()` return `true` for integers too.



## Integer overflows



`JsonVariantConst::is<T>()` is aware of integer overflows and only returns `true` if the specified type can store the value.



For example if the variant contains `512`, `is<char>()` returns `false`, but `is<int>()` return `true`.



## User-defined types



`JsonVariantConst::is<T>()` supports user-defined types by calling `canConvertFromJson()`. For example, to support an hypothetical `Complex` class, we could write:



```cpp
bool canConvertFromJson(JsonVariantConst src, const Complex&) {
  return src["real"].is<double>() && src["imag"].is<double>();
}
```



The second parameter of `canConvertFromJson()` is required to trigger [ADL](https://en.cppreference.com/w/cpp/language/adl) but must not be used by the function.



## See also


* [`JsonVariant::is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/)


---


# JsonVariantConst::nesting()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariantconst/nesting/

## Description



`JsonVariantConst::nesting()` returns the depth (i.e., the nesting level) of the value pointed by the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariant/)



| JSON | Nesting |
| --- | --- |
| `null` | 0 |
| `"string"` | 0 |
| `[0]` | 1 |
| `[[0]]` | 2 |
| `{"a":0}` | 1 |
| `{"a":{"b":0}}` | 2 |
| `{"a":{"b":[0]}}` | 3 |



## Signature



```cpp
size_t nesting() const;
```



## See also


* [`JsonDocument::nesting()`](https://arduinojson.org/v7/api/jsondocument/nesting/)


---


# JsonVariantConst::operator[]
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariantconst/subscript/

## Description



`JsonVariantConst::operator[]` gets a value in the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/).



Depending on the argument type the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) can be used like an [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/) or a [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/).



`JsonVariantConst::operator[]` is a read-only version of [`JsonVariant::operator[]`](https://arduinojson.org/v7/api/jsonvariant/subscript/).



## Signatures



```cpp
// mimics a JsonArrayConst
JsonVariantConst operator[](size_t index) const;

// mimics a JsonObjectConst
JsonVariantConst operator[](const char* key) const;
JsonVariantConst operator[](char* key) const;
JsonVariantConst operator[](const String& key) const;
JsonVariantConst operator[](const std::string& key) const;
JsonVariantConst operator[](std::string_view key) const;
JsonVariantConst operator[](const __FlashStringHelper* key) const;
```



## Arguments



`index`: the index in the [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/).



`key`: the key in the [`JsonObject`](https://arduinojson.org/v7/api/jsonobject/)



## Return value



A [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) that points to the object member or the array element (depending on the overload you called).



The returned [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) can be null (i.e [`JsonVariantConst::isNull()`](https://arduinojson.org/v7/api/jsonvariantconst/isnull/) returns `true`) if the index is out of range, the key is not in the object, or the original [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) is null.



## See also


* [`JsonVariant::operator[]`](https://arduinojson.org/v7/api/jsonvariant/subscript/)


---


# JsonVariantConst::size()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonvariantconst/size/

## Description



If the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) points to an object, `JsonVariantConst::size()` returns the number of key-value pairs in the object (same as [`JsonObjectConst::size()`](https://arduinojson.org/v7/api/jsonobjectconst/size/))



If the [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/) points to an array, `JsonVariantConst::size()` returns the number of elements in the array (same as [`JsonArrayConst::size()`](https://arduinojson.org/v7/api/jsonarrayconst/size/))



In all other cases, `JsonVariantConst::size()` returns `0`



Internally, this function walks a linked-list to count the elements, so its [time complexity](https://en.wikipedia.org/wiki/Time_complexity) is *O(n)*.



## Signature



```cpp
size_t size() const;
```



## See also


* [`JsonArrayConst::size()`](https://arduinojson.org/v7/api/jsonarrayconst/size/)
* [`JsonObjectConst::size()`](https://arduinojson.org/v7/api/jsonobjectconst/size/)


---


# JsonArrayConst::begin() / JsonArrayConst::end()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarrayconst/begin_end/

## Description



The member functions `begin()` and `end()` return STL-style iterators. You can use these iterators to enumerate all the elements in the array pointed by the [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/).



These functions reproduce the containers in the C++ Standard Library and allow you to use the “ranged-based for loop” feature of C++11. See the example below.



## Signatures



```cpp
JsonArrayConst::iterator begin() const;
JsonArrayConst::iterator end() const;
```



## Return value



`begin()` returns an iterator to the first element of the array.



`end()` returns an iterator to the element after the last. This iterator must not be dereferenced because it is out of the array. It’s a placeholder to detect the end of the array.



`JsonArrayConst::iterator` points to a [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/). You can dereference the iterator with the usual `*` and `->` operators.



## Example



```cpp
char json[] = "[\"one\",\"two\",\"three\"]";
JsonDocument doc;
deserializeJson(doc, json);
JsonArrayConst arr = doc.as<JsonArray>();

for (JsonVariantConst value : arr) {
    Serial.println(value.as<const char*>());
}
```



The code above would print:



```cpp
one
two
three
```



## See also


* [`JsonArray::begin()` / `JsonArray::end()`](https://arduinojson.org/v7/api/jsonarray/begin_end/)
* To recursively walk the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), see the “Recursive Analyzer” case study in [Mastering ArduinoJson](https://arduinojson.org/book/)


---


# JsonArrayConst::isNull()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarrayconst/isnull/

## Description



[`JsonArrayConst::isNull()`](https://arduinojson.org/v7/api/jsonarrayconst/isnull/) tells whether the [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/) points to an array or not.



You can use this function to:


1. check if the array was successfully parsed, or
2. check if the array was successfully allocated.



As an alternative, you can use the conversion to `bool`; for example, `if(array)` instead of `if(!array.isNull())`



## Signature



```cpp
bool isNull() const;
```



## Return value



[`JsonArrayConst::isNull()`](https://arduinojson.org/v7/api/jsonarrayconst/isnull/) returns a `bool` that tells if the [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/) points to something:


* `true` if the [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/) is null,
* `false` if the [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/) is valid and points to an array.



## See also


* [`JsonArray`](https://arduinojson.org/v7/api/jsonarray/)


---


# JsonArrayConst::nesting()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarrayconst/nesting/

## Description



`JsonArrayConst::nesting()` returns the depth (i.e., the nesting level) of the array.



| JSON | Nesting |
| --- | --- |
| `null` | 0 |
| `[0]` | 1 |
| `[[0]]` | 2 |
| `[{"a":0}]` | 2 |
| `[{"a":[0]}]` | 3 |



## Signature



```cpp
size_t nesting() const;
```



## See also


* [`JsonDocument::nesting()`](https://arduinojson.org/v7/api/jsondocument/nesting/)


---


# JsonArrayConst::size()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonarrayconst/size/

## Description



[`JsonArrayConst::size()`](https://arduinojson.org/v7/api/jsonarrayconst/size/) gets the number of elements in the array pointed by the [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/).



If the [`JsonArrayConst`](https://arduinojson.org/v7/api/jsonarrayconst/) is null, this function returns `0`.



Internally, this function walks a linked-list to count the elements, so its [time complexity](https://en.wikipedia.org/wiki/Time_complexity) is *O(n)*.



Don’t use this function to create a `for` loop; instead, use [iterators](https://arduinojson.org/v7/api/jsonarrayconst/begin_end/).



## Signature



```cpp
size_t size() const;
```



## Return value



[`JsonArrayConst::size()`](https://arduinojson.org/v7/api/jsonarrayconst/size/) returns an unsigned integer containing the number of elements in the array.


---


# JsonObjectConst::begin() / JsonObjectConst::end()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobjectconst/begin_end/

## Description



The member functions `begin()` and `end()` return STL-style iterators. You can use these iterators to enumerate all the key-value pairs in the object pointed by the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/).



These functions reproduce the containers in the C++ Standard Library and allow you to use the “ranged-based for loop” feature of C++11.



## Signatures



```cpp
JsonObjectConst::iterator begin() const;
JsonObjectConst::iterator end() const;
```



## Return value



`begin()` returns an iterator to the first key-value pair of the object.



`end()` returns an iterator to the element after the last. This iterator must not be dereferenced; it’s a placeholder to detect the end of the object.



`JsonObjectConst::iterator` points to a `JsonPairConst`, a class that bundles a key (accessible via `JsonPair::key()`) and a value (accessible via `JsonPair::value()`).



The type of the key is [`JsonString`](https://arduinojson.org/v7/api/jsonstring/); you need to call `JsonString::c_str()` to get the char-pointer out of it. The type of the value is [`JsonVariantConst`](https://arduinojson.org/v7/api/jsonvariantconst/).



Here is a summary:



```cpp
class JsonPairConst {
public:
  JsonString key() const;
  JsonVariantConst value() const;
};

class JsonString {
public:
  const char* c_str() const;
};
```



## See also


* [`JsonObject::begin()` / `JsonObject::end()`](https://arduinojson.org/v7/api/jsonobject/begin_end/)
* To recursively walk the [`JsonDocument`](https://arduinojson.org/v7/api/jsondocument/), see the “Recursive Analyzer” case study in [Mastering ArduinoJson](https://arduinojson.org/book/)


---


# JsonObjectConst::containsKey()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobjectconst/containskey/

> > ## Deprecated



> `containsKey()` is deprecated since ArduinoJson 7.2 and you must use [`is<T>()`](https://arduinojson.org/v7/api/jsonvariant/is/) instead.



> ```cpp
// Before
if (doc.containsKey("value")) {
  int value = doc["value"];
  // ...
}

// After
if (doc["value"].is<int>()) {
  int value = doc["value"];
  // ...
}
> ```



> If you want to check that a key exists regardless of its type, you can use `is<JsonVariantConst>()`.



## Description



[`JsonObjectConst::containsKey()`](https://arduinojson.org/v7/api/jsonobjectconst/containskey/) tests whether a key exists in the object pointed by the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/).



If the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/) is null, this function returns `false`.



This function can (and should) be avoided most of the time. See below.



## Signature



```cpp
bool containsKey(const char* key) const;
bool containsKey(const String& key) const;
bool containsKey(const std::string& key) const;
bool containsKey(const __FlashStringHelper& key) const;
bool containsKey(std::string_view key) const;
```



## Arguments



`key`: the key to look for.



## Return value



[`JsonObjectConst::containsKey()`](https://arduinojson.org/v7/api/jsonobjectconst/containskey/) returns a `bool` that tells whether the key was found or not:


* `true` if the key is present in the object
* `false` if the key is absent of the object



## Example



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

JsonObjectConst location = doc["location"];
bool hasCity = location.containsKey("city"); // true
bool hasCountry = location.containsKey("country"); // false
```



## Avoid this function when you can!



This function can (and should) be avoided most of the time.



Because ArduinoJson implements the [Null Object Pattern](https://en.wikipedia.org/wiki/Null_Object_pattern), it is always safe to read the object: if the key doesn’t exist, it returns an empty value. For example:



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

JsonObjectConst location = doc["location"];
if (location.containsKey("city")) {
  const char* city = location["city"];
  Serial.println(city);
}
```



Can be written like this:



```cpp
JsonDocument doc;
doc["location"]["city"] = "Paris";

const char* city = doc["location"]["city"];
if (city)
  Serial.println(city);
```



This version should lead to a **smaller** and **faster** code since it only does the lookup once.



## How to test nested keys?



You cannot test the presence of nested keys with `containsKey()` but, as explained above, it’s safe to read the object anyway.



For example, when [Weather Underground](http://www.wunderground.com) returns an error like:



```cpp
{
  "response": {
    "version": "0.1",
    "termsofService": "http://www.wunderground.com/weather/api/d/terms.html",
    "features": {
      "conditions": 1
    },
    "error": {
      "type": "querynotfound",
      "description": "No cities match your search query"
    }
  }
}
```



You should not try to call `containsKey("response")`, `containsKey("error")` and `containsKey("description")`. Instead, just get the value and test if it’s null:



```cpp
const char* error = doc["response"]["error"]["description"];
if (error) {
  Serial.println(error);
  return;
}
```



## See also


* [`JsonObject::containsKey()`](https://arduinojson.org/v7/api/jsonobject/containskey/)


---


# JsonObjectConst::isNull()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobjectconst/isnull/

## Description



[`JsonObjectConst::isNull()`](https://arduinojson.org/v7/api/jsonobjectconst/isnull/) tests whether the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/) points to an object or not.



You can use this function to:


1. check if the object was successfully parsed, or
2. check if the object was successfully allocated.



As an alternative, you can use the conversion to `bool`; for example, `if(object)` instead of `if(!object.isNull())`



## Signature



```cpp
bool isNull() const;
```



## Return value



[`JsonObjectConst::isNull()`](https://arduinojson.org/v7/api/jsonobjectconst/isnull/) returns a `bool` that tells if the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/) points to something:


* `true` if the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/) is null,
* `false` if the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/) is valid and points to an object.



## See also


* [`JsonObject::isNull()`](https://arduinojson.org/v7/api/jsonobject/isnull/)


---


# JsonObjectConst::nesting()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobjectconst/nesting/

## Description



`JsonObjectConst::nesting()` returns the depth (i.e., the nesting level) of the object pointed by the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonvariant/)



| JSON | Nesting |
| --- | --- |
| `null` | 0 |
| `{"a":0}` | 1 |
| `{"a":{"b":0}}` | 2 |
| `{"a":{"b":[0]}}` | 3 |



## Signature



```cpp
size_t nesting() const;
```



## See also


* [`JsonDocument::nesting()`](https://arduinojson.org/v7/api/jsondocument/nesting/)


---


# JsonObjectConst::size()
 | ArduinoJson 7

Source: https://arduinojson.org/v7/api/jsonobjectconst/size/

## Description



[`JsonObjectConst::size()`](https://arduinojson.org/v7/api/jsonobjectconst/size/) returns the number of key-value pairs in the object pointed by the [`JsonObjectConst`](https://arduinojson.org/v7/api/jsonobjectconst/).



Internally, this function walks a linked-list to count the elements, so its [time complexity](https://en.wikipedia.org/wiki/Time_complexity) is *O(n)*.



If you want to loop over each key-value pair in the object, use [iterators](https://arduinojson.org/v7/api/jsonobjectconst/begin_end/).



## Signature



```cpp
size_t size() const;
```



## Return value



[`JsonObjectConst::size()`](https://arduinojson.org/v7/api/jsonobjectconst/size/) return an unsigned integer that contains the number of key-value pairs in the object.



## See also


* [`JsonObject::size()`](https://arduinojson.org/v7/api/jsonobject/size/)


---
