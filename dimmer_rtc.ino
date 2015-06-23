/*
Programa: Dimmer com Controle de Horário - RTC
Acionamento de um dimmer com Triac e módulo RTC para controlar o
horario de acionamento em tempo real.
*/
#include <Wire.h>
#include <stdio.h>
#include "Nanoshield_RTC.h"

Nanoshield_RTC rtc;

//valores em horas
const int hora_ativacao = 10;
const int hora_desligamento = 23;
const int tempo_de_transicao = 1;

int triac = 3; // Triac: pino D3
int zc = 2;    // Detetor de zero: pino D2

// Tempo entre a detecção de zero e o acionamento do triac
int time;

void setup()
{
  // Inicializa os pinos como entrada ou saída
  pinMode(triac, OUTPUT);
  pinMode(zc, INPUT);

  // Associa a borda de descida do detetor de zero com
  //   a função dimmer(), que aciona o triac
  attachInterrupt(0, dimmer, FALLING);
}

void loop()
{
  //  executa a funcao de ativacao pelo RTC
  //   para o delay de acionamento do triac (de 1 a 7331us).
  int t = controla_acionamento();

  // Atualiza a variável "time", desabilitando as interrupções
  //   para que não haja conflito no acesso.
  noInterrupts();
  time = t;
  interrupts();
}

void dimmer()
{
  // Espera até o momento correto (variável "time") e aciona o triac
  delayMicroseconds(time);
  digitalWrite(triac, HIGH);

  // Gera a borda de descida do pulso do triac com largura de 200us
  delayMicroseconds(200);
  digitalWrite(triac, LOW);
}

int controla_acionamento() {
  //controla ativacao
  if (rtc.getHours() >= hora_ativacao && rtc.getHours() <= (hora_ativacao + tempo_de_transicao)) {
    //mapeia e retorna o valor a cada minuto
    int t = map(rtc.getMinutes(), 0, 59, 0, 1023);
    return t* 7.1652 + 1;
  }

  //controla valor intermediario
  if (rtc.getHours() >= (hora_ativacao + tempo_de_transicao) && rtc.getHours() < hora_desligamento) {
    int t = 1023*7.1652 + 1;
    return t;
  }
  
  //controla valor desligamento
  if (rtc.getHours() >= hora_desligamento && rtc.getHours() <= hora_desligamento + tempo_de_transicao) {
    //mapeia e retorna o valor a cada minuto
    int t = map(rtc.getMinutes(), 0, 60, 0, 1023);
    return t* 7.1652 + 1;
  }
  else{ return 0;}
}
