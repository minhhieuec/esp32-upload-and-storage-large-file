#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ESP32WebServer.h>

#define USE_LittleFS

#include <FS.h>
#ifdef USE_LittleFS
#define SPIFFS LITTLEFS
#include <LITTLEFS.h>
#else
#include <SPIFFS.h>
#endif

#define FORMAT_LITTLEFS_IF_FAILED true

ESP32WebServer server(80);
String webpage = "";

const char ap_ssid[] = "ESP32";
const char ap_pwd[] = "12345678";

void readFile(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while (file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- file written");
    }
    else
    {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("- failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- message appended");
    }
    else
    {
        Serial.println("- append failed");
    }
    file.close();
}

void File_Upload()
{
    webpage += F("<h3>Select File to Upload</h3>");
    webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
    webpage += F("<input class='buttons' style='width:40%' type='file' name='fupload' id = 'fupload' value=''><br>");
    webpage += F("<br><button class='buttons' style='width:20%' type='submit'>Upload File</button><br>");

    server.send(200, "text/html", webpage);
}

void SendHTML_Header()
{
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    // append_page_header();
    server.sendContent(webpage);
    webpage = "";
}

void SendHTML_Content()
{
    server.sendContent(webpage);
    webpage = "";
}

void SendHTML_Stop()
{
    server.sendContent("");
    server.client().stop(); // Stop is needed because no content length was sent
}

void ReportCouldNotCreateFile(String target)
{
    SendHTML_Header();
    webpage += F("<h3>Could Not Create Uploaded File (write-protected?)</h3>");
    webpage += F("<a href='/");
    webpage += target + "'>[Back]</a><br><br>";
    // append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();
}

File UploadFile;
int cnt = 0;
void handleFileUpload()
{
    // upload a new file to the Filing system
    HTTPUpload &uploadfile = server.upload(); // See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                              // For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
    if (uploadfile.status == UPLOAD_FILE_START)
    {
        String filename = uploadfile.filename;
        if (!filename.startsWith("/"))
            filename = "/" + filename;
        Serial.print("Upload File Name: ");
        Serial.println(filename);
        // SD.remove(filename);                         // Remove a previous version, otherwise data is appended the file again
        // UploadFile = SD.open(filename, FILE_WRITE);  // Open the file for writing in SPIFFS (create it, if doesn't exist)
        // filename = String();
        Serial.print("total Size: ");
        Serial.println(uploadfile.totalSize);
        Serial.print("current Size: ");
        Serial.println(uploadfile.currentSize);
        uploadfile.status = UPLOAD_FILE_WRITE;
    }
    else if (uploadfile.status == UPLOAD_FILE_WRITE)
    {
        cnt++;
        Serial.println(cnt);
        // if(UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
    }
    else if (uploadfile.status == UPLOAD_FILE_END)
    {
        Serial.println("UPLOAD_FILE_END");

        if (UploadFile) // If the file was successfully created
        {
            UploadFile.close(); // Close the file again
            Serial.print("Upload Size: ");
            Serial.println(uploadfile.totalSize);
            webpage = "";
            // append_page_header();
            webpage += F("<h3>File was successfully uploaded</h3>");
            webpage += F("<h2>Uploaded File Name: ");
            webpage += uploadfile.filename + "</h2>";
            webpage += F("<h2>File Size: ");
            // webpage += file_size(uploadfile.totalSize) + "</h2><br>";
            // append_page_footer();
            server.send(200, "text/html", webpage);
        }
        else
        {
            ReportCouldNotCreateFile("upload");
        }
    }
}

void setup(void)
{
    Serial.begin(115200);

    if (!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        Serial.println("LITTLEFS Mount Failed");
        return;
    }

    readFile(LITTLEFS, "/data.txt");

    // You can remove the password parameter if you want the AP to be open.
    WiFi.softAP(ap_ssid, ap_pwd);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    /* server init */
    server.on("/upload", File_Upload);
    server.on(
        "/fupload", HTTP_POST, []()
        { server.send(200); },
        handleFileUpload);
    server.begin();
}

void loop(void)
{
    server.handleClient(); // Listen for client connections
}