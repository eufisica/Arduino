// Street light and crosswalk - José Gonçalves (eufisica)


int CarroVermelho = 8;

int CarroAmarelo = 9;

int CarroVerde = 10;

int PeaoVermelho = 3;

int PeaoVerde = 2;

int botao = 7;

int TempoPeao = 10000;               //Tempo para os peões atravessarem a passadeira

unsigned long MudaTempoBotao = 0;    //Tempo desde que o botão foi pressionado


void setup()

{

  pinMode(CarroVermelho, OUTPUT);

  pinMode(CarroAmarelo, OUTPUT);

  pinMode(CarroVerde, OUTPUT);

  pinMode(PeaoVermelho, OUTPUT);

  pinMode(PeaoVerde, OUTPUT);

  pinMode(botao, INPUT_PULLUP);

  digitalWrite(CarroVerde, HIGH);

  digitalWrite(PeaoVermelho, HIGH);

}


void loop()

{

  int estado = digitalRead(botao);

  if(estado == 0 && (millis() - MudaTempoBotao) > 3000)    //Verifica se o botão foi pressionado

  {                                                        //e se decorreram 5 segundos desde a

    mudarLED();                                            //última vez que isso ocorreu

  }

}


void mudarLED()

{

  digitalWrite(CarroAmarelo, HIGH);

  digitalWrite(CarroVerde, LOW);

  delay(2000);

  digitalWrite(CarroVermelho, HIGH);

  digitalWrite(CarroAmarelo, LOW);

  delay(1000);

  digitalWrite(PeaoVerde, HIGH);

  digitalWrite(PeaoVermelho, LOW);

  delay(TempoPeao);

  for (int x=0; x<9; x++)            //Pisca o led verde dos peões

  {

    digitalWrite(PeaoVerde, LOW);

    delay(250);

    digitalWrite(PeaoVerde, HIGH);

    delay(250);

  }

  digitalWrite(PeaoVermelho, HIGH);

  digitalWrite(PeaoVerde, LOW);

  delay(1000);

  digitalWrite(CarroVerde, HIGH);

  digitalWrite(CarroVermelho, LOW);

  MudaTempoBotao = millis();         //Regista o tempo desde a última alteração no semáforo

}
