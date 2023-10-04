//#include <WiFiClientSecure.h>    -  https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/ // 


//#include <UniversalTelegramBot.h>  - https://www.arduinolibraries.info/libraries/universal-telegram-bot//


//#include <ArduinoJson.h>  - baixar biblioteca arduinojson no proprio arduino// 



// Dados do WiFi
#define WIFI_SSID ""
#define WIFI_PASSWORD ""


// Use @myidbot (IDBot) para saber qual o seu ID
#define CHAT_ID ""



// Telegram BOT Token (Botfather)
#define BOT_TOKEN ""



X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

const unsigned long BOT_MTBS = 1000; tempo entre leitura das mensagens
unsigned long bot_lasttime;


#define B D5
#define G D8
#define R D6
#define rele D0
#define Pinbuzzer D1
#define valorlimite 400 ////////////valor que após ser passado indica vazamento
#define Sensor A0 


bool flag = 1;
bool alarme = 1; ////////////// O alarme ja começa ligado automaticamente, ao ligar o dispositivo
bool mensagens = 1; /////////////////// as mensagens também


unsigned long tempoanterior = 0;
unsigned long tempoanterior2 = 0;
unsigned long tempoanterior3 = 0;


void handleNewMessages(int numNewMessages) ///////////leitura das mensagens 
{

  int leiturasensor = analogRead(Sensor);
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

   

    if (text == "/ajuda")
    {
      String welcome = "Olá, " + from_name + ", bem vindo ao Safe Gas BOT!\n";
      welcome += "Aqui voce poderá monitorar a situação do gás da sua cozinha em qualquer lugar e a qualquer hora!\n\n\n";
      welcome += "- Basta digitar \"/status\", e você receberá uma mensagem, indicando se a situação do gás está OK ou se está ocorrendo vazamento.\n\n";
      welcome += "- Para receber mensagens automáticas em caso de vazamento, envie \"/ligarmsg\" para desligar esta função, use o comando \"/desligarmsg\". Vale ressaltar que se retirado da tomada, ligue as mensagens ao energizar novamente, para garantir que serão recebidas.\n\n";
      welcome += "- Você também poderá desligar o buzzer e o led do Safe Gas digitando \"/desligaralarme\", para ligar o alarme de volta basta enviar \"/ligaralarme\".";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
else if (text == "/status" and leiturasensor > valorlimite)
    {
      String status1 = "Olá, " + from_name + ", o seu gás está vazando!";
      bot.sendMessage(CHAT_ID, status1, "Markdown");
    }

    else if (text == "/ligarmsg" or text == "/ligarmensagem")
{
  mensagens = 1;
    bot.sendMessage(CHAT_ID, "As mensagens foram ativadas!", "");
  }

else if (text == "/desligarmsg" or text == "/desligarmensagem")

{

 mensagens = 0;
    bot.sendMessage(CHAT_ID, "As mensagens foram desativadas!", "");

}

else if (text == "/desligaralarme")

{
 
alarme= 0;
  bot.sendMessage(CHAT_ID, "O alarme foi desligado!", "");

}

else if (text == "/ligaralarme")

{

alarme = 1;
  bot.sendMessage(CHAT_ID, "O alarme foi ligado!", "");

}
else if (text == "/status" and leiturasensor < valorlimite)

  {
      String status2 = "Olá, " + from_name + ", o seu gás está OK!";
      bot.sendMessage(CHAT_ID, status2, "Markdown");
    }

  else //////////////////se receber qualquer outra mensagem
  {
  bot.sendMessage(chat_id, "Comando desconhecido! Digite \"/ajuda\" para receber os comandos disponíveis.");
  }
 
  
}


}


void setup() {

  pinMode(Sensor, INPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(Pinbuzzer, OUTPUT);
  pinMode(rele, OUTPUT);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(Pinbuzzer, LOW);
  
  Serial.begin(9600);
  Serial.println();

 // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot.sendMessage(CHAT_ID, "O Safe Gas iniciou!", "");
}

void loop() {


int leiturasensor = analogRead(Sensor);


if (millis() - tempoanterior >= 1000){
  tempoanterior= millis();
  Serial.println(leiturasensor);
  Serial.println("- - - - -");

                                     }


if (leiturasensor > valorlimite) {

  digitalWrite(rele, HIGH);

if (alarme == 1)

{
 digitalWrite(Pinbuzzer, HIGH); 
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(B, HIGH);
}



if (mensagens == 1 )
{
    if (millis() - tempoanterior2 >= 10000 )
    {
      tempoanterior2 = millis();
      String vazamento = "O gás da sua cozinha está vazando! \n\n";
      vazamento += "Abra as portas e as janelas da sua casa!\n\n";
      vazamento += "Se possível, feche o registro do gás!\n\n";
      vazamento += "Não acenda a luz ou use eletrodomésticos até que o problema seja corrigido!";
      bot.sendMessage(CHAT_ID, vazamento, "Markdown");   
    }

} 
    flag = 1; 
                                 }

  else {


  digitalWrite(rele, LOW);


  if (alarme == 1){
    digitalWrite(Pinbuzzer, LOW);//BUZZER DESLIGADO
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);

  }

  if (alarme == 0){
    digitalWrite(Pinbuzzer, LOW);//BUZZER DESLIGADO
    digitalWrite(R, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(B, HIGH);


  }

    if (flag == 1 and mensagens == 1) {
      bot.sendMessage(CHAT_ID, "O seu gás está OK!", "");
      flag = 0;
              }

        }
  

   if (millis() - bot_lasttime > 1000)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }


  }
