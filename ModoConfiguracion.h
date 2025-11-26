#ifndef MODO_CONFIGURACION_H
#define MODO_CONFIGURACION_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include "html.h"
#include "config.h"

AsyncWebServer server(80);
extern bool ini_PC_zero;

void ledsModoConfiguracion() {
    for (int i = 0; i < NUM_PULSADORES; i++) {
        pinMode(PINES_LEDS[i], OUTPUT);
        digitalWrite(PINES_LEDS[i], HIGH);
        delay(200);
        digitalWrite(PINES_LEDS[i], LOW);
        delay(200);
    }
}

String generarPulsadores() {
    String html = "";
    for (int i = 0; i < NUM_PULSADORES; i++) {
        int valorPulsador = EEPROM.read(ADDR_PULSADORES + i);
        int valorCCpulsador = EEPROM.read(ADDR_CC_PULS + i);
        int modoEEPROM = EEPROM.read(ADDR_MODO_PULS + i);
        String modoActual = (modoEEPROM == 1) ? "CC" : (modoEEPROM == 2 ? "RL" : "PC");
        int valor = (modoActual == "PC") ? valorPulsador : (modoActual == "CC" ? valorCCpulsador : 0);

        html += "<div style='display:flex;align-items:center;margin-bottom:10px;'>";
        html += "<label style='font-size:12px;font-weight:bold;color:#8e44ad;margin-left:20px;margin-right:10px;'>PULSADOR " + String(i + 1) + "</label>";
        html += "<select name='modo_" + String(i) + "' onchange='actualizarCampoValor(" + String(i) + ")' style='margin-right:10px;'>";
        html += "<option value='PC'" + String(modoActual == "PC" ? " selected" : "") + ">PC</option>";
        html += "<option value='CC'" + String(modoActual == "CC" ? " selected" : "") + ">CC</option>";
        html += "<option value='RL'" + String(modoActual == "RL" ? " selected" : "") + ">RL</option>";
        html += "</select>";
        html += "<input type='number' id='valor_" + String(i) + "' name='valor_" + String(i) + "' min='1' max='128' value='" + String(valor) + "' style='width:45px;margin-right:20px;" + String(modoActual == "RL" ? "background:#ccc;' disabled" : "'") + ">";
        for (int r = 0; r < NUM_RELES; r++) {
            bool estado = EEPROM.read(ADDR_RELES + (i * NUM_RELES) + r) == 1;
            html += "<label style='font-size:0.8em;color:#fff;margin-right:10px;display:inline-flex;align-items:center;'>";
            html += "<input type='checkbox' name='r" + String(r + 1) + "_" + String(i) + "' value='1' " + (estado ? "checked" : "") + " style='accent-color:#8e44ad;'> Relé " + String(r + 1) + "</label>";
        }
        html += "</div>";
    }

    for (int i = 0; i < NUM_EXPRESION; i++) {
        int valorExp = EEPROM.read(ADDR_CC_EXPRESION + i);
        html += "<div style='display:flex;align-items:center;margin-bottom:10px;'>";
        html += "<label style='font-size:12px;font-weight:bold;color:#8e44ad;margin-left:20px;margin-right:10px;'>CC PEDAL EXPRESIÓN " + String(i + 1) + "</label>";
        html += "<input type='number' name='EXP_" + String(i) + "' min='0' max='127' value='" + String(valorExp) + "' style='width:45px;margin-left:12px;'>";
        html += "</div>";
    }

    html += "<div style='margin-left:20px;margin-bottom:20px;'>";
    html += "<label style='font-size:12px;font-weight:bold;color:#fff;display:inline-flex;align-items:center;'>";
    html += "<input type='checkbox' name='zero' value='1' " + String(EEPROM.read(ADDR_ZERO) == 1 ? "checked" : "") + " style='accent-color:#8e44ad;margin-right:6px;'> Iniciar PC en 0 (mensaje MIDI PC-1)";
    html += "</label></div>";

    html += R"rawliteral(
<script>
function actualizarCampoValor(i){
  var modo=document.getElementsByName('modo_'+i)[0].value;
  var campo=document.getElementById('valor_'+i);
  if(modo==='RL'){
    campo.disabled=true;
    campo.style.background='#ccc';
    campo.value='';
  }else{
    campo.disabled=false;
    campo.style.background='';
  }
}
</script>
)rawliteral";

    return html;
}

void iniciarServidorWeb() {
    EEPROM.begin(1024);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.setHostname(HOST_NAME);
    MDNS.begin(mdns);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = paginaHTML;
        html.replace("%PULSADORES%", generarPulsadores());
        request->send(200, "text/html; charset=UTF-8", html);
    });

    server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
        ini_PC_zero = request->hasParam("zero");
        EEPROM.write(ADDR_ZERO, ini_PC_zero ? 1 : 0);

        for (int i = 0; i < NUM_PULSADORES; i++) {
            String Modo = "modo_" + String(i);
            String Numero = "valor_" + String(i);
            String modo = request->hasParam(Modo) ? request->getParam(Modo)->value() : "PC";
            int valor = request->hasParam(Numero) ? request->getParam(Numero)->value().toInt() : 0;

            if (modo == "RL") {
                bool algunRele = false;
                for (int r = 0; r < NUM_RELES; r++) {
                    if (request->hasParam("r" + String(r + 1) + "_" + String(i))) {
                        algunRele = true;
                        break;
                    }
                }
                if (!algunRele) break;
                EEPROM.write(ADDR_PULSADORES + i, 0);
                EEPROM.write(ADDR_CC_PULS + i, 0);
                EEPROM.write(ADDR_MODO_PULS + i, 2);
            } else {
                if (valor < 1) valor = 1;
                if (valor > 128) valor = 128;
                if (modo == "PC") {
                    EEPROM.write(ADDR_PULSADORES + i, (uint8_t)valor);
                    EEPROM.write(ADDR_CC_PULS + i, 0);
                    EEPROM.write(ADDR_MODO_PULS + i, 0);
                } else {
                    EEPROM.write(ADDR_CC_PULS + i, (uint8_t)valor);
                    EEPROM.write(ADDR_PULSADORES + i, 0);
                    EEPROM.write(ADDR_MODO_PULS + i, 1);
                }
            }
            for (int r = 0; r < NUM_RELES; r++) {
                EEPROM.write(ADDR_RELES + (i * NUM_RELES) + r, request->hasParam("r" + String(r + 1) + "_" + String(i)) ? 1 : 0);
            }
        }

        for (int i = 0; i < NUM_EXPRESION; i++) {
            if (request->hasParam("EXP_" + String(i))) {
                int valor = request->getParam("EXP_" + String(i))->value().toInt();
                if (valor < 0) valor = 0;
                if (valor > 127) valor = 127;
                EEPROM.write(ADDR_CC_EXPRESION + i, (uint8_t)valor);
            }
        }

        EEPROM.commit();
        String html = paginaHTML;
        html.replace("%PULSADORES%", generarPulsadores());
        request->send(200, "text/html; charset=UTF-8", "<script>alert('¡Configuración guardada!'); window.location.href = '/';</script>" + html);
    });

    server.begin();
}

void Modo_Configuracion() {
    iniciarServidorWeb();
    while (true) {
        ledsModoConfiguracion();
    }
}

#endif
