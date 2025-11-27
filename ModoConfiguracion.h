#ifndef MODO_CONFIGURACION_H
#define MODO_CONFIGURACION_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include "html.h"
#include "config.h"

AsyncWebServer server(80);
unsigned long lastBlinkMillis = 0;
bool ledState = false;

void ledsModoConfiguracion() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastBlinkMillis >= 400) {
        lastBlinkMillis = currentMillis;
        ledState = !ledState;
        for (int i = 0; i < NUM_PULSADORES; i++) digitalWrite(PINES_LEDS[i], ledState ? HIGH : LOW);
    }
}

String generarPerfilesTabs(int seleccionado) {
    String html = "";
    for (int p = 0; p < NUM_PERFILES; p++) {
        String cls = (p == seleccionado) ? "tab active" : "tab";
        html += "<div class='" + cls + "' onclick=\"openTab(event,'perfil_" + String(p)
             + "');document.getElementById('perfilSel').value='" + String(p) + "'\">Perfil "
             + String(p + 1) + "</div>";
    }
    return html;
}

String generarPulsadoresPorPerfil(int perfil, int perfilActivo) {
    String html = "";
    String contClass = (perfil == perfilActivo) ? "tabcontent active" : "tabcontent";
    html += "<div id='perfil_" + String(perfil) + "' class='" + contClass + "'>";
    for (int i = 0; i < NUM_PULSADORES; i++) {
        int valorPulsador = EEPROM.read(ADDR_PULSADORES(perfil) + i);
        int valorCCpulsador = EEPROM.read(ADDR_CC_PULS(perfil) + i);
        int modoEEPROM = EEPROM.read(ADDR_MODO_PULS(perfil) + i);
        String modoActual = (modoEEPROM == 1) ? "CC" : (modoEEPROM == 2 ? "RL" : "PC");
        int valor = (modoActual == "PC") ? valorPulsador : (modoActual == "CC" ? valorCCpulsador : 0);
        html += "<div style='display:flex;align-items:center;margin-bottom:10px;'>";
        html += "<label style='font-size:12px;font-weight:bold;color:#8e44ad;margin-left:20px;margin-right:10px;'>PULSADOR " + String(i + 1) + "</label>";
        html += "<select name='modo_" + String(perfil) + "_" + String(i) + "' onchange='actualizarCampoValor(" + String(perfil) + "," + String(i) + ")' style='margin-right:10px;'>";
        html += "<option value='PC'" + String(modoActual == "PC" ? " selected" : "") + ">PC</option>";
        html += "<option value='CC'" + String(modoActual == "CC" ? " selected" : "") + ">CC</option>";
        html += "<option value='RL'" + String(modoActual == "RL" ? " selected" : "") + ">RL</option>";
        html += "</select>";
        html += "<input type='number' id='valor_" + String(perfil) + "_" + String(i) + "' name='valor_" + String(perfil) + "_" + String(i) + "' min='0' max='127' value='" + String(valor) + "' style='width:45px;margin-right:20px;" + String(modoActual == "RL" ? "background:#ccc;' disabled" : "'") + ">";
        for (int r = 0; r < NUM_RELES; r++) {
            bool estado = EEPROM.read(ADDR_RELES(perfil) + (i * NUM_RELES) + r) == 1;
            html += "<label style='font-size:0.8em;color:#fff;margin-right:10px;display:inline-flex;align-items:center;'>";
            html += "<input type='checkbox' name='r" + String(r + 1) + "_" + String(perfil) + "_" + String(i) + "' value='1' " + (estado ? "checked" : "") + " style='accent-color:#8e44ad;margin-right:6px;'> Relé " + String(r + 1) + "</label>";
        }
        html += "</div>";
    }
    html += "</div>";
    return html;
}

void iniciarServidorWeb() {
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.setHostname(HOST_NAME);
    MDNS.begin(mdns);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        int perfilSel = request->hasParam("perfilSel") ? request->getParam("perfilSel")->value().toInt() : 0;
        String html = paginaHTML;
        html.replace("%PERFILES%", generarPerfilesTabs(perfilSel));
        String contenidos = "";
        for (int p = 0; p < NUM_PERFILES; p++) contenidos += generarPulsadoresPorPerfil(p, perfilSel);
        html.replace("%PULSADORES%", contenidos);
        request->send(200, "text/html; charset=UTF-8", html);
    });

    server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
        bool error = false;
        for (int perfil = 0; perfil < NUM_PERFILES; perfil++) {
            for (int i = 0; i < NUM_PULSADORES; i++) {
                String Modo = "modo_" + String(perfil) + "_" + String(i);
                String Numero = "valor_" + String(perfil) + "_" + String(i);
                String modo = request->hasParam(Modo) ? request->getParam(Modo)->value() : "PC";
                int valor = request->hasParam(Numero) ? request->getParam(Numero)->value().toInt() : 0;
                if (modo == "RL") {
                    bool algunRele = false;
                    for (int r = 0; r < NUM_RELES; r++)
                        if (request->hasParam("r" + String(r + 1) + "_" + String(perfil) + "_" + String(i)))
                            algunRele = true;
                    if (!algunRele) error = true;
                    EEPROM.write(ADDR_PULSADORES(perfil) + i, 0);
                    EEPROM.write(ADDR_CC_PULS(perfil) + i, 0);
                    EEPROM.write(ADDR_MODO_PULS(perfil) + i, 2);
                } else if (modo == "PC") {
                    if (valor < 1) valor = 1;
                    EEPROM.write(ADDR_PULSADORES(perfil) + i, valor);
                    EEPROM.write(ADDR_CC_PULS(perfil) + i, 0);
                    EEPROM.write(ADDR_MODO_PULS(perfil) + i, 0);
                } else if (modo == "CC") {
                    if (valor < 0) valor = 0;
                    if (valor > 127) valor = 127;
                    EEPROM.write(ADDR_CC_PULS(perfil) + i, valor);
                    EEPROM.write(ADDR_PULSADORES(perfil) + i, 0);
                    EEPROM.write(ADDR_MODO_PULS(perfil) + i, 1);
                }
                for (int r = 0; r < NUM_RELES; r++) {
                    bool estado = request->hasParam("r" + String(r + 1) + "_" + String(perfil) + "_" + String(i));
                    EEPROM.write(ADDR_RELES(perfil) + (i * NUM_RELES) + r, estado ? 1 : 0);
                }
            }
            for (int i = 0; i < NUM_EXPRESION; i++) {
                String Exp = "EXP_" + String(perfil) + "_" + String(i);
                if (request->hasParam(Exp)) {
                    int valor = request->getParam(Exp)->value().toInt();
                    if (valor < 0) valor = 0;
                    if (valor > 127) valor = 127;
                    EEPROM.write(ADDR_CC_EXPRESION(perfil) + i, valor);
                }
            }
            bool iniPCZero = request->hasParam("zero_" + String(perfil));
            EEPROM.write(ADDR_PC_CERO(perfil), iniPCZero ? 1 : 0);
        }
        EEPROM.commit();
        String html = paginaHTML;
        html.replace("%PERFILES%", generarPerfilesTabs(0));
        String contenidos = "";
        for (int p = 0; p < NUM_PERFILES; p++) contenidos += generarPulsadoresPorPerfil(p, 0);
        html.replace("%PULSADORES%", contenidos);
        String htmlFinal = error ? "<script>alert('Error: en modo RL seleccione al menos un relé'); window.location.href = '/';</script>" + html
                                  : "<script>alert('¡Configuración guardada!'); window.location.href = '/';</script>" + html;
        request->send(200, "text/html; charset=UTF-8", htmlFinal);
    });

    server.begin();
}

void Modo_Configuracion() {
    EEPROM.begin(1024);
    for (int i = 0; i < NUM_PULSADORES; i++) {
        pinMode(PINES_LEDS[i], OUTPUT);
        digitalWrite(PINES_LEDS[i], LOW);
    }
    iniciarServidorWeb();
    while (true) {
        ledsModoConfiguracion();
        delay(10);
    }
}

#endif
