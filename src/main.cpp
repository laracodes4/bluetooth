// código bluetooth slave -----------------------------------

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Bounce2.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>

BluetoothSerial BT;
Bounce botao = Bounce();
// F4:65:0B:47:55:E6
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define pindht 18
#define tipodht DHT22
#define ledVm 4
unsigned long intervalo = 2000;
unsigned long intervalo2 = 3000;
unsigned long marcoZero = 0;
unsigned long marcoZero1 = 0;
bool estadoLED = false;
bool piscando;
String modoLed;
bool estadoBotao;
bool estadoAtualBotao;
bool estadoAnteriorBotao;
bool pressionado;
DHT sensor(pindht, tipodht);

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(5000);
  pinMode(ledVm, OUTPUT);
  botao.attach(22, INPUT_PULLUP);
  sensor.begin();
  lcd.init();
  lcd.backlight();

  if (BT.begin("BtSlaveLara"))
  { // verificação pra ver se o bluetooth tá com defeito
    Serial.println("Bluetooth iniciado com sucesso");
    Serial.print("Endereco bluetooth: ");
    Serial.println(BT.getBtAddressString());
  }
  else
  {
    Serial.println("Erro ao iniciar o bluetooth");
  }
}

void loop()
{

  // BLUETOOTH COM BOTÃO ===========================================

  //   unsigned long marcoAtual = millis();

  //   if (BT.available())
  //   {                                                      // verifica se chegou algo no bluetooth -- if(xx == true) = if (xx)
  //     String msgRecebida = BT.readStringUntil('\n');       // função que lê o que está vindo pro bluetooth até um caractere específico, \n significa final de linha (enter)
  //     msgRecebida.trim();                                  //.trim remove caracteres em branco ou especiais
  //     Serial.printf("Mensagem recebida: %s", msgRecebida); // printf serve pra juntar texto com valor de variável, %s é pra String (assim como %i seria pra int)

  //     if (msgRecebida == "l" || msgRecebida == "L")
  //     {
  //       piscando = false;
  //       digitalWrite(ledVm, HIGH);
  //     }
  //     if (msgRecebida == "d" || msgRecebida == "D")
  //     {
  //       piscando = false;
  //       digitalWrite(ledVm, LOW);
  //     }
  //     if (msgRecebida == "p" || msgRecebida == "P")
  //     {
  //       piscando = true;
  //     }
  //   }

  //   if (piscando)
  //   {
  //     if (marcoAtual - marcoZero >= intervalo)
  //     {

  //       if (estadoLED == LOW)
  //       {
  //         estadoLED = HIGH;
  //       }
  //       else
  //       {
  //         estadoLED = LOW;
  //       }
  //       digitalWrite(ledVm, estadoLED);

  //       marcoZero = marcoAtual;
  //     }
  //   }

  //   // if (Serial.available()){ // verificando se tem algo na minha serial (processo inverso do bloco acima)
  //   //  String msgEnviar = Serial.readStringUntil('\n');
  //   // msgEnviar.trim();

  //   botao.update();

  // unsigned long marcoAtual1 = millis();

  // if (botao.fell())
  // {
  //     marcoZero1 = marcoAtual1;
  // }

  // if (botao.read() == LOW && marcoAtual1 - marcoZero1 >= 3000)
  // {
  //     if (!pressionado)
  //     {
  //         BT.println("P");
  //         Serial.println("Enviado: P");
  //         pressionado = true;
  //     }
  // }

  // if (botao.rose())
  // {
  //     if (!pressionado)
  //     {
  //         estadoLED = !estadoLED;

  //         if (estadoLED)
  //         {
  //             BT.println("L");
  //             Serial.println("Enviado: L");
  //         }
  //         else
  //         {
  //             BT.println("D");
  //             Serial.println("Enviado: D");
  //         }
  //     }

  //     pressionado = false;
  // }

  // DHT E LCD NO BLUETOOTH ======================================

  // enviar dados -------------------------------

  // if (Serial.available())
  // {
  // String msgEnviar = Serial.readStringUntil('\n');
  // msgEnviar.trim();

  float tempC = sensor.readTemperature();
  float umidade = sensor.readHumidity();

  unsigned long marcoAtual = millis();
  static float ultimaTempC = -999;
  static float ultimaUmid = -999;

  if (marcoAtual - marcoZero >= intervalo)
  {
    if (tempC != ultimaTempC)
    {
      BT.print(tempC, 1);
      BT.print(",");

      ultimaTempC = tempC;
    }
    else{
      BT.print(ultimaTempC, 1);
      BT.print(",");

      ultimaTempC = tempC;
    }

     if (umidade != ultimaUmid)
    {
      BT.print(umidade, 1);

      ultimaUmid = umidade;
    }
    else{
      BT.print(ultimaUmid, 1);

      ultimaUmid = umidade;
    }

      //------------minha serial ------------
      Serial.print("Temp C = ");
      Serial.println(ultimaTempC);
      Serial.print("Umidade = ");
      Serial.println(ultimaUmid);
      Serial.println("---------------");

      marcoZero = marcoAtual;
    }

    // receber dados ----------------------------------

    if (BT.available())
    {
      String msgRecebida = BT.readStringUntil('\n');

      // if (BT.available())
      // {
      //   String msgRecebida = BT.readStringUntil('\n');
      //   msgRecebida.trim();
      //   Serial.printf("Mensagem recebida: %s", msgRecebida);

      int separador = msgRecebida.indexOf(',');

      if (separador != -1)
      {
        float tempCb = msgRecebida.substring(0, separador).toFloat();
        float umidadeb = msgRecebida.substring(separador + 1).toFloat();

        lcd.setCursor(0, 0);
        lcd.print("Temp em C = ");
        lcd.print(tempCb);

        lcd.setCursor(0, 1);
        lcd.print("Umidade = ");
        lcd.print(umidadeb);
      }
    }
  }

  // código bluetooth master ----------------------------

  // #include <Arduino.h>
  // #include <BluetoothSerial.h>

  // BluetoothSerial BT;
  // // F4:65:0B:47:FA:9E CÓDIGO GABRIEL
  // // F4:65:0B:47:55:E6 //tenho que colocar endereço do colega, pois ele que vai receber

  // uint8_t endSlave[] = {0xF4, 0x65, 0x0B, 0x47, 0xFA, 0x9E};

  // void setup9(){
  //   Serial.begin (115200);
  //   Serial.setTimeout(5000);

  //   if (BT.begin("BtMasterLara", true)){
  //     Serial.println("Bluetooth iniciado com sucesso");
  //   }
  //   else {
  //     Serial.println("Erro ao iniciar bluetooth");
  //   }

  //   if(BT.connect(endSlave)){
  //     Serial.println("Conectado com sucesso");
  //   }
  //   else{
  //   Serial.println("Erro ao conectar");
  //   }
  // }

  // void loop(){
  // if (BT.available()){ //verifica se chegou algo no bluetooth -- if(xx == true) = if (xx)
  //   String msgRecebida = BT.readStringUntil('\n'); //função que lê o que está vindo pro bluetooth até um caractere específico, \n significa final de linha (enter)
  //   msgRecebida.trim(); //.trim remove caracteres em branco ou especiais
  //   Serial.printf("Mensagem recebida: %s", msgRecebida); //printf serve pra juntar texto com valor de variável, %s é pra String (assim como %i seria pra int)
  //  }

  //  if (Serial.available()){ //vericando se tem algo na minha serial (processo inverso do bloco acima)
  //   String msgEnviar = Serial.readStringUntil('\n');
  //   msgEnviar.trim();
  //   BT.println(msgEnviar); //envia o que tem na serial por bluetooth, três linhas acima não servem se o dado estiver no sensor
  //  }
  //}