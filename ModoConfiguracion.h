#ifndef MODO_CONFIGURACION_H
#define MODO_CONFIGURACION_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include "html.h"
#include "config.h"

AsyncWebServer server(80);

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
        String modoActual = (modoEEPROM == 1) ? "CC" : "PC";
        int valor = (modoActual == "PC") ? valorPulsador : valorCCpulsador;
        html += "<div style='display: flex; align-items: center; margin-bottom: 10px;'>";
        html += "<label style='font-size: 12px; font-weight: bold; color: #8e44ad; margin-left: 20px; margin-right: 10px;'>PULSADOR " + String(i + 1) + "</label>";
        html += "<select name='modo_" + String(i) + "' style='margin-right: 10px;'>";
        html += "<option value='PC'" + String(modoActual == "PC" ? " selected" : "") + ">PC</option>";
        html += "<option value='CC'" + String(modoActual == "CC" ? " selected" : "") + ">CC</option>";
        html += "</select>";
        html += "<input type='number' name='valor_" + String(i) + "' min='1' max='128' value='" + String(valor) + "' style='width: 40px; margin-right: 20px;'>";
        for (int r = 0; r < NUM_RELES; r++) {
            bool estado = EEPROM.read(ADDR_RELES + (i * NUM_RELES) + r) == 1;
            html += "<label style='font-size: 0.8em; color: #ddd; margin-right: 10px;'><input type='checkbox' name='r" + String(r + 1) + "_" + String(i) + "' value='1' " + (estado ? "checked" : "") + "> Relé " + String(r + 1) + "</label>";
        }
        html += "</div>";
    }
    for (int i = 0; i < NUM_EXPRESION; i++) {
        int valorExp = EEPROM.read(ADDR_CC_EXPRESION + i);
        html += "<div style='display: flex; align-items: center; margin-bottom: 10px;'>";
        html += "<label style='font-size:12px; font-weight:bold; color:#8e44ad; margin-left:20px; margin-right:8px;'>CC PEDAL EXPRESIÓN " + String(i + 1) + "</label>";
        html += "<input type='number' name='EXP_" + String(i) + "' min='0' max='127' value='" + String(valorExp) + "' style='width:40px;'>";
        html += "</div>";
    }
    return html;
}

void iniciarServidorWeb() {
    EEPROM.begin(512);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.setHostname(HOST_NAME);
    Serial.println("Punto de acceso WiFi iniciado");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.softAPIP());
    MDNS.begin(mdns);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = paginaHTML;
        html.replace("%PULSADORES%", generarPulsadores());
        request->send(200, "text/html; charset=UTF-8", html);
    });

    server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
        for (int i = 0; i < NUM_PULSADORES; i++) {
            String Modo = "modo_" + String(i);
            String Numero = "valor_" + String(i);
            if (request->hasParam(Modo) && request->hasParam(Numero)) {
                String modo = request->getParam(Modo)->value();
                int valor = request->getParam(Numero)->value().toInt();
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
        request->send(200, "text/html; charset=UTF-8", "<script>alert('¡Configuración guardada!'); window.location.href = '/';</script>");
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
