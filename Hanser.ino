#include <ESP8266WiFi.h> 
#include <ESP8266HTTPClient.h> 
#include <ArduinoJson.h>
#include <DYWiFiConfig.h>
#include "Ticker.h" //引入调度器头文件

Ticker myTicker; //建立一个需要定时调度的对象
DYWiFiConfig wificonfig;
ESP8266WebServer webserver(80);
#define DEF_WIFI_SSID        "306Lifi"
#define DEF_WIWI_PASSWORD     "sss11223344."
#define AP_NAME           "FunnyChip" //dev
char Address[7] = "110000";
char Key[33] = "ac2f3457cc2d7928a8b4600e9759be1a";
//char Bid[9] = "49890113";
char Bid[20] = "49890113";
int WeatherCount = 3;

char  assid[20]=     "306Lifi";
char  apassword[22]= "sss11223344.";

#define IntTime     1

#define ESP_WeatherNum      0x400
#define ESP_Weather       0x401
#define ESP_Temp        0x402
#define ESP_Humi        0x403
#define ESP_Wind_Dir      0x404
#define ESP_Wind_Pw       0x405
#define ESP_Report_Tm     0x406

#define ESP_D1_WeatherNum   0x410
#define ESP_D1_Week       0x411
#define ESP_D1_Weather      0x412
#define ESP_D1_Temp       0x413
#define ESP_D1_NTemp      0x414

#define ESP_D2_WeatherNum   0x420
#define ESP_D2_Week       0x421
#define ESP_D2_Weather      0x422
#define ESP_D2_Temp       0x423
#define ESP_D2_NTemp      0x424

#define ESP_D3_WeatherNum   0x430
#define ESP_D3_Week       0x431
#define ESP_D3_Weather      0x432
#define ESP_D3_Temp       0x433
#define ESP_D3_NTemp      0x434

#define ESP_Year        0x501
#define ESP_Month         0x502
#define ESP_Day           0x503
#define ESP_Week        0x504
#define ESP_Hour          0x505
#define ESP_Minute        0x506
#define ESP_Second          0x507

#define ESP_Bili_Msg        0x511
#define ESP_Bili_Fow        0x512

#define CMD_Address       0x8011
#define CMD_BiliID        0x8012
#define CMD_Weather       0x8013

#define ESP_KEY_MODE 0x601
#define ESP_KEY_MENU 0x602
#define ESP_KEY_SW 0x603

unsigned int display_year = 0, display_month = 0, display_day = 0, display_week = 0, display_hour = 0, display_minute = 0, display_second = 0;
String Week[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

void SendString(unsigned int Address, String Data)
{
  unsigned char SendString[] = { 0xFF, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  SendString[2] = (Address >> 8) & 0xff;
  SendString[3] = (Address)& 0xff;
  SendString[4] = Data.length();
  for (int i = 0; i < SendString[4]; i++)
  {
    SendString[5 + i] = Data[i];
  }
  Serial.write(SendString, 5 + SendString[4]);
}

void SendInter(unsigned int Address, unsigned int Data)
{
  unsigned char SendInter[] = { 0xFF, 0x55, 0x00, 0x00, 0x02, 0x00, 0x00 };

  SendInter[2] = (Address >> 8) & 0xff;
  SendInter[3] = (Address)& 0xff;

  SendInter[5] = (Data >> 8) & 0xff;
  SendInter[6] = (Data)& 0xff;

  Serial.write(SendInter, 7);
}

u16 ConvertWeatherNum(String data_content)
{
  int i;
  String weather_phenomena1[] = { "晴", "少云", "晴间多云", "多云", "阴", "有风", "平静", "微风", "和风", "清风", "强风/劲风", "疾风", "大风", "烈风", "风暴",
    "狂爆风", "飓风", "热带风暴", "阵雨", "雷阵雨", "雷阵雨并伴有冰雹", "小雨", "中雨", "大雨", "暴雨", "大暴雨", "特大暴雨",
    "强阵雨", "强雷阵雨", "极端降雨", "毛毛雨/细雨", "雨", "小雨-中雨", "中雨-大雨", "大雨-暴雨", "暴雨-大暴雨", "大暴雨-特大暴雨",
    "雨雪天气", "雨夹雪", "阵雨夹雪", "冻雨", "雪", "阵雪", "小雪", "中雪", "大雪", "暴雪", "小雪-中雪", "中雪-大雪", "大雪-暴雪",
    "浮尘", "扬沙", "沙尘暴", "强沙尘暴", "龙卷风", "雾", "浓雾", "强浓雾", "轻雾", "大雾", "特强浓雾", "霾", "中度霾", "重度霾",
    "严重霾", "热", "冷", "未知" };
  for (i = 0; i < 68; i++) {

    //Serial.println(data_content+" "+ weather_phenomena1[i]);

    if (data_content.equals(weather_phenomena1[i])) {
      return i;
    }
  }
}

String ConvertWeather(String data_content)
{
  int i;
  String weather_phenomena1[] = { "晴", "少云", "晴间多云", "多云", "阴", "有风", "平静", "微风", "和风", "清风", "强风/劲风", "疾风", "大风", "烈风", "风暴",
    "狂爆风", "飓风", "热带风暴", "阵雨", "雷阵雨", "雷阵雨并伴有冰雹", "小雨", "中雨", "大雨", "暴雨", "大暴雨", "特大暴雨",
    "强阵雨", "强雷阵雨", "极端降雨", "毛毛雨/细雨", "雨", "小雨-中雨", "中雨-大雨", "大雨-暴雨", "暴雨-大暴雨", "大暴雨-特大暴雨",
    "雨雪天气", "雨夹雪", "阵雨夹雪", "冻雨", "雪", "阵雪", "小雪", "中雪", "大雪", "暴雪", "小雪-中雪", "中雪-大雪", "大雪-暴雪",
    "浮尘", "扬沙", "沙尘暴", "强沙尘暴", "龙卷风", "雾", "浓雾", "强浓雾", "轻雾", "大雾", "特强浓雾", "霾", "中度霾", "重度霾",
    "严重霾", "热", "冷", "未知" };
  for (i = 0; i < 68; i++) {

    //Serial.println(data_content+" "+ weather_phenomena1[i]);

    if (data_content.equals(weather_phenomena1[i])) {
      break;
    }
  }
  if (i == 0) {
    return "Sunny";  //晴
  }
  else if (i >= 1 && i <= 3) {
    return "Cloudy";  //多云
  }
  else if (i == 4) {
    return "Cast";   //阴
  }
  else if (i >= 5 && i <= 17) {
    return "Gale";    //风
  }
  else if (i >= 18 && i <= 40) {
    return "Rain";   //雨
  }
  else if (i >= 41 && i <= 49) {
    return "Snow";   //雪
  }
  else if (i >= 50 && i <= 54) {
    return "Dust";  //沙尘
  }
  else if (i >= 55 && i <= 60) {
    return "Fog";   //雾
  }
  else if (i >= 61 && i <= 64) {
    return "Haze";   //霾
  }
  else {
    return "Unknown";   //未知
  }
}

String ConvertWindDir(String data_content)
{
  int i;
  String direction_phenomena1[] = { "无风向", "东北", "东", "东南", "南", "西南", "西", "西北", "北", "旋转不定", "未知" };
  for (i = 0; i < 11; i++) {
    if (data_content.equals(direction_phenomena1[i])) {
      break;
    }
  }
  switch (i)
  {
  case 0:return "Breezeless";
  case 1:return "Northeast";
  case 2:return "East";
  case 3:return "Southeast";
  case 4:return "South";
  case 5:return "Southwest";
  case 6:return "West";
  case 7:return "Northwest";
  case 8:return "North";
  case 9:return "Uncertain";
  case 10:return "Unknown";
  }
}

//****获取天气子函数 
void get_weather() {
  SendString(CMD_Address, Address);
  delay(IntTime);
  if (WiFi.status() == WL_CONNECTED) { //如果 Wi-Fi 连接成功
    //此处往下是取得实况天气的程序
    HTTPClient http;  //开始登陆 
    //不要使用和下面相同的秘钥

    const char* HOST = "http://restapi.amap.com/v3/weather";
    String GetUrl = String(HOST) + "/weatherInfo?parameters&key=";
    GetUrl += String(Key);
    GetUrl += "&city=";
    GetUrl += String(Address);
    GetUrl += "&extensions=";

    http.begin(GetUrl + "base");  //高德开放平台提供服务 
    int httpget_now = http.GET(); //赋值
    if (httpget_now > 0) { //检查一下是否为0，应该是去检查缓存区是否为空
      /*数据解析:使用 https://arduinojson.org/assistant/ 一个工具可以直接生成程序，挑有用的复制就行*/
      const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(9) + 210;
      DynamicJsonBuffer jsonBuffer(capacity);

      JsonObject& root = jsonBuffer.parseObject(http.getString());

      JsonObject& lives_0 = root["lives"][0];
      const char* lives_0_weather = lives_0["weather"]; // "天气情况"
      const char* lives_0_temperature = lives_0["temperature"]; // "温度"
      const char* lives_0_humidity = lives_0["humidity"]; // "风力"
      const char* lives_0_winddirection = lives_0["winddirection"]; // "风向"
      const char* lives_0_windpower = lives_0["windpower"]; // "风力"
      const char* lives_0_reporttime = lives_0["reporttime"]; // "风力"

      //赋值，因为现在这些变量是在缓存区，一会将被清空
      String now_weather = lives_0_weather;    //当前天气
      String now_temperature = lives_0_temperature;    //当前温度
      String now_humidity = lives_0_humidity;    //当前温度
      String now_wind_direction = lives_0_winddirection;   //当前风向
      String now_wind_power = lives_0_windpower;   //当前风力
      String now_reporttime = lives_0_reporttime;   //当前风力
      now_reporttime = now_reporttime.substring(5, 19);   //当前风力
      String display_wind_power = now_wind_power.substring(3, 6);

      SendInter(ESP_WeatherNum, ConvertWeatherNum(now_weather));
      delay(IntTime);
      SendString(ESP_Weather, ConvertWeather(now_weather));
      delay(IntTime);
      SendString(ESP_Temp, now_temperature);
      delay(IntTime);
      SendString(ESP_Humi, now_humidity);
      delay(IntTime);
      SendString(ESP_Wind_Dir, ConvertWindDir(now_wind_direction));
      delay(IntTime);
      SendString(ESP_Wind_Pw, display_wind_power);
      delay(IntTime);
      SendString(ESP_Report_Tm, now_reporttime);
      delay(IntTime);
    }
    http.end();
    delay(50);

    //此处往下是取得未来三天的天气的程序
    http.begin(GetUrl + "all");  //高德开放平台提供服务 
    int httpget_threedays = http.GET(); //赋值 
    if (httpget_threedays > 0) { //检查一下是否为0，应该是去检查缓存区是否为空
      const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(4) + 2 * JSON_OBJECT_SIZE(5) + 4 * JSON_OBJECT_SIZE(10) + 690;
      DynamicJsonBuffer jsonBuffer(capacity);
      JsonObject& root = jsonBuffer.parseObject(http.getString());
      JsonObject& forecasts_0 = root["forecasts"][0];
      JsonArray& forecasts_0_casts = forecasts_0["casts"];

      JsonObject& forecasts_0_casts_1 = forecasts_0_casts[1]; //明天天气
      const char* forecasts_0_casts_1_week = forecasts_0_casts_1["week"]; // "1"
      const char* forecasts_0_casts_1_dayweather = forecasts_0_casts_1["dayweather"]; // "多云"
      const char* forecasts_0_casts_1_daytemp = forecasts_0_casts_1["daytemp"]; // "22"
      const char* forecasts_0_casts_1_nighttemp = forecasts_0_casts_1["nighttemp"]; // "10"

      JsonObject& forecasts_0_casts_2 = forecasts_0_casts[2]; //后天天气
      const char* forecasts_0_casts_2_week = forecasts_0_casts_2["week"]; // "2"
      const char* forecasts_0_casts_2_dayweather = forecasts_0_casts_2["dayweather"]; // "晴"
      const char* forecasts_0_casts_2_daytemp = forecasts_0_casts_2["daytemp"]; // "19"
      const char* forecasts_0_casts_2_nighttemp = forecasts_0_casts_2["nighttemp"]; // "10"

      JsonObject& forecasts_0_casts_3 = forecasts_0_casts[3]; //大后天天气
      const char* forecasts_0_casts_3_week = forecasts_0_casts_3["week"]; // "3"
      const char* forecasts_0_casts_3_dayweather = forecasts_0_casts_3["dayweather"]; // "晴"
      const char* forecasts_0_casts_3_daytemp = forecasts_0_casts_3["daytemp"]; // "25"
      const char* forecasts_0_casts_3_nighttemp = forecasts_0_casts_3["nighttemp"]; // "14"

      //****赋值
      //第一天数据
      String first_week = forecasts_0_casts_1_week;
      String first_dayweather = forecasts_0_casts_1_dayweather;
      String first_daytemp = forecasts_0_casts_1_daytemp;
      String first_nighttemp = forecasts_0_casts_1_nighttemp;


      SendInter(ESP_D1_WeatherNum, ConvertWeatherNum(first_dayweather));
      delay(IntTime);
      SendInter(ESP_D1_Week, first_week.toInt());
      delay(IntTime);
      SendString(ESP_D1_Weather, ConvertWeather(first_dayweather));
      delay(IntTime);
      SendString(ESP_D1_Temp, first_daytemp);
      delay(IntTime);
      SendString(ESP_D1_NTemp, first_nighttemp);
      delay(IntTime);

      //第二天数据
      String second_week = forecasts_0_casts_2_week;
      String second_dayweather = forecasts_0_casts_2_dayweather;
      String second_daytemp = forecasts_0_casts_2_daytemp;
      String second_nighttemp = forecasts_0_casts_2_nighttemp;


      SendInter(ESP_D2_WeatherNum, ConvertWeatherNum(second_dayweather));
      delay(IntTime);
      SendInter(ESP_D2_Week, second_week.toInt());
      delay(IntTime);
      SendString(ESP_D2_Weather, ConvertWeather(second_dayweather));
      delay(IntTime);
      SendString(ESP_D2_Temp, second_daytemp);
      delay(IntTime);
      SendString(ESP_D2_NTemp, second_nighttemp);
      delay(IntTime);

      //第三天数据
      String third_week = forecasts_0_casts_3_week;
      String third_dayweather = forecasts_0_casts_3_dayweather;
      String third_daytemp = forecasts_0_casts_3_daytemp;
      String third_nighttemp = forecasts_0_casts_3_nighttemp;


      SendInter(ESP_D3_WeatherNum, ConvertWeatherNum(third_dayweather));
      delay(IntTime);
      SendInter(ESP_D3_Week, third_week.toInt());
      delay(IntTime);
      SendString(ESP_D3_Weather, ConvertWeather(third_dayweather));
      delay(IntTime);
      SendString(ESP_D3_Temp, third_daytemp);
      delay(IntTime);
      SendString(ESP_D3_NTemp, third_nighttemp);
      delay(IntTime);

    }
    http.end();   //关闭与服务器的连接
    delay(10);
  }
}

int week(int y, int m, int d)
{
  if (m == 1 || m == 2) m += 12, y = y - 1;
  return (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7;
}
//****获取时间子函数
void get_time() {
  if (WiFi.status() == WL_CONNECTED) { //如果 Wi-Fi 连接成功
    HTTPClient http;  //开始登陆 
    //不要使用和下面相同的秘钥
    http.begin("http://quan.suning.com/getSysTime.do");  //苏宁提供服务 
    int httpCode = http.GET(); //赋值                               
    if (httpCode > 0) { //检查一下是否为0，应该是去检查缓存区是否为空

      /*数据解析  //使用 https://arduinojson.org/assistant/ 一个工具可以直接生成程序，挑有用的复制就行*/
      const size_t capacity = JSON_OBJECT_SIZE(2) + 60;
      DynamicJsonBuffer jsonBuffer(capacity);

      JsonObject& root = jsonBuffer.parseObject(http.getString());

      const char* sysTime1 = root["sysTime1"]; // "20190419131920"
      String now_time = sysTime1;
      display_year = (now_time.substring(0, 4)).toInt();
      display_month = (now_time.substring(4, 6)).toInt();
      display_day = (now_time.substring(6, 8)).toInt();
      display_hour = (now_time.substring(8, 10)).toInt();
      display_minute = (now_time.substring(10, 12)).toInt();
      display_second = (now_time.substring(12, 14)).toInt();
      display_week = week(display_year, display_month, display_day);   //蔡勒公式

      SendInter(ESP_Year, display_year);
      delay(IntTime);
      SendInter(ESP_Month, display_month);
      delay(IntTime);
      SendInter(ESP_Day, display_day);
      delay(IntTime);
      SendInter(ESP_Week, (display_week + 7) % 7);
      delay(IntTime);
      SendInter(ESP_Hour, display_hour);
      delay(IntTime);
      SendInter(ESP_Minute, display_minute);
      delay(IntTime);
      SendInter(ESP_Second, display_second);

    }
    http.end();
    delay(10);
  }
}
void get_fans() {
  if (WiFi.status() == WL_CONNECTED) { //如果 Wi-Fi 连接成功
    HTTPClient http;  //开始登陆 
    //不要使用和下面相同的秘钥

    const char* HOST = "http://api.bilibili.com";
    String GetUrl = String(HOST) + "/x/relation/stat?vmid=";
    GetUrl += String(Bid);

    http.begin(GetUrl);
    int httpCode = http.GET();

    if (httpCode == 200) {
      //Serial.println("Get OK");
      String resBuff = http.getString();
      DynamicJsonBuffer jsonBuffer(200);
      JsonObject& root = jsonBuffer.parseObject(resBuff);

      String msg = root["message"];
      uint message = msg.toInt();;
      String fans = root["data"]["follower"];

      SendInter(ESP_Bili_Msg, message);
      delay(IntTime);
      //      SendInter(ESP_Bili_Fow, fans);

      SendString(ESP_Bili_Fow, fans);
      delay(IntTime);
      //Serial.print(message);
    }
    http.end();
  }
  delay(10);
}
// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
Name:       Hanser.ino
Created:  2019/9/13 17:53:08
Author:     DESKTOP-KE6HV16\77249
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//


void connectwifi() {
  wificonfig.begin(&webserver, "/");
  DYWIFICONFIG_STRUCT defaultConfig = wificonfig.createConfig();
  strcpy(defaultConfig.SSID, DEF_WIFI_SSID);
  strcpy(defaultConfig.SSID_PASSWORD, DEF_WIWI_PASSWORD);
  strcpy(defaultConfig.HOSTNAME, AP_NAME);
  strcpy(defaultConfig.APNAME, AP_NAME);
  wificonfig.setDefaultConfig(defaultConfig);
  wificonfig.enableAP();
}

#define DATA_PACKAGE_MIN_LEN    5
#define DATA_PACKAGE_MAX_LEN    512
#define DATA_PACKAGE_FFT_LEN    200
// 同步帧头
#define CMD_HEAD1 0xFF
#define CMD_HEAD2 0x55

#define MAKEWORD(low, high)    (((byte)(low)) | (((byte)(high)) << 8))

byte Uart_Data[DATA_PACKAGE_MAX_LEN] = { 0xFF, 0x55, 0x00, 0x00, 0x02, 0x00, 0x00 };
bool Uart_Overflow_Flag = false;


void USART_Handler(void)                   //串口1中断服务程序
{
  byte Uart_Recv_Data = 0;
  static byte Uart_Recv_Step = 0;
  static byte Uart_Recv_Count = 0;

  while (Serial.available() > 0)
  {
    Uart_Recv_Data = (byte)Serial.read();
//    Serial.println(Uart_Recv_Data);
    if (!Uart_Overflow_Flag)
    {
      switch(Uart_Recv_Step)
      {
        case 0:if(Uart_Recv_Data==CMD_HEAD1) Uart_Recv_Step++; break;
        case 1:if(Uart_Recv_Data==CMD_HEAD2) Uart_Recv_Step++; else Uart_Recv_Step=0;break;
        case 2:Uart_Data[2] = Uart_Recv_Data; Uart_Recv_Step++;break;
        case 3:Uart_Data[3] = Uart_Recv_Data; Uart_Recv_Step++;break;
        case 4:Uart_Data[4] = Uart_Recv_Data; Uart_Recv_Step++;break;
        case 5:Uart_Data[Uart_Recv_Count + DATA_PACKAGE_MIN_LEN] = Uart_Recv_Data; Uart_Recv_Count++;if(Uart_Recv_Count>=Uart_Data[4]) {Uart_Recv_Step = 0;Uart_Recv_Count = 0;AnalysisYousamsg(Uart_Data);}break;
      }
    }
  }
}

void AnalysisYousamsg(uint8_t *Buf)
{
  int i;
  //  if(Buf[0] == CMD_HEAD1 &&Buf[1] == CMD_HEAD2)
  {
    {
      switch (MAKEWORD(Buf[3], Buf[2]))
      {
      case CMD_Address: for (i = 0; i < Buf[4]; i++) Address[i] = Buf[i + 5]; WeatherCount = 1; break;
      case CMD_BiliID:memset(&Bid, 0, sizeof(Bid)); for (i = 0; i<Buf[4]; i++) Bid[i] = Buf[i + 5]; break;
      case CMD_Weather: WeatherCount = MAKEWORD(Buf[6], Buf[5]); break;
      }
    }
  }
}

#define KEY_MENU 5
#define KEY_SW 4
#define KEY_MODE 0

void ICACHE_RAM_ATTR keyHandle();
void ICACHE_RAM_ATTR tickerHandle();
// The setup() function runs once each time the micro-controller starts
void setup()
{
  Serial.begin(115200);
  connectwifi();
  Serial.println("Connecting..");
  while (WiFi.status() != WL_CONNECTED)
  { //等待wifi连接
    wificonfig.handle();
//  delay(2);
//  Serial.println("Connecting");
  }
  Serial.println("Connected");
  wificonfig.disableAP();
//WiFi.begin(assid,apassword);
  pinMode(KEY_MENU, INPUT_PULLUP);
  attachInterrupt(KEY_MENU, keyHandle, CHANGE);
  pinMode(KEY_SW, INPUT_PULLUP);
  attachInterrupt(KEY_SW, keyHandle, CHANGE);
  pinMode(KEY_MODE, INPUT_PULLUP);
  attachInterrupt(KEY_MODE, keyHandle, CHANGE);

  get_time(); //开机取一次时间
  get_weather(); //开机取一次天气
  
    myTicker.attach(1, tickerHandle); //初始化调度任务，每0.5秒执行一次tickerHandle()
}

int keypress = 0;
int shake = 0;

void keyHandle() {
  if (shake<1)
  {
    if (digitalRead(KEY_SW) == LOW)
    {
      shake = 60;
      keypress = 1;
    }
    if (digitalRead(KEY_MENU) == LOW)
    {
      shake = 60;
      keypress = 2;
    }
    if (digitalRead(KEY_MODE) == LOW)
    {
      shake = 60;
      keypress = 3;
    }
  }
}

void get_key()
{
  switch (keypress)
  {
  case 1:SendInter(ESP_KEY_SW, 1); break;
  case 2:SendInter(ESP_KEY_MENU, 1); break;
  case 3:SendInter(ESP_KEY_MODE, 1); break;
  }
  keypress = 0;
}
bool TimeFlag = false;

void tickerHandle() //到时间时需要执行的任务
{
//    Serial.println(millis()); //打印当前时间
  TimeFlag = true;
}

long ResetCount = 0;
// Add the main program code into the continuous loop() function
void loop()
{
  if (ResetCount++ % 360000 == 5)
    WeatherCount = 1;
  if (shake>0)
    shake--;
  get_key();//检测按键按下情况
  if (TimeFlag)
  {
    TimeFlag = false;
    get_time();//获取时间
    get_fans();//获取数据
  }
  if (WeatherCount > 0)
  {
    WeatherCount--;
    get_weather();//获取天气
  }
  USART_Handler();
  delay(2);
}
