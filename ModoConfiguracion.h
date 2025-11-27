#ifndef MODO_CONFIGURACION_H
#define MODO_CONFIGURACION_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include "html.h"
#include "config.h"

AsyncWebServer server(80);
unsigned long lastBlinkMillis = 0;
bool ledState = false;

void ledsModoConfiguracion() {
    unsigned long currentMillis = millis();
    if(currentMillis - lastBlinkMillis >= 400){
        lastBlinkMillis = currentMillis;
        ledState = !ledState;
        for(int i=0;i<NUM_PULSADORES;i++) digitalWrite(PINES_LEDS[i], ledState?HIGH:LOW);
    }
}

String generarPerfilesTabs(int seleccionado){
    String html = "";
    for(int p=0;p<NUM_PERFILES;p++){
        String cls = (p==seleccionado)?"tab active":"tab";
        html += "<div class='" + cls + "' onclick=\"openTab(event,'perfil_" + String(p) + "');document.getElementById('perfilSel').value='" + String(p) + "'\">Perfil " + String(p+1) + "</div>";
    }
    return html;
}

String generarPulsadoresPorPerfil(int perfil,int perfilActivo){
    String html="";
    String contClass = (perfil==perfilActivo)?"tabcontent active":"tabcontent";
    html += "<div id='perfil_"+String(perfil)+"' class='"+contClass+"'>";
    for(int i=0;i<NUM_PULSADORES;i++){
        int valorPulsador = EEPROM.read(ADDR_PULSADORES(perfil)+i);
        int valorCC = EEPROM.read(ADDR_CC_PULS(perfil)+i);
        int modoEEPROM = EEPROM.read(ADDR_MODO_PULS(perfil)+i);
        String modoActual = (modoEEPROM==1)?"CC":(modoEEPROM==2?"RL":"PC");
        int valor = (modoActual=="PC")?valorPulsador:(modoActual=="CC"?valorCC:0);

        html += "<div style='display:flex;align-items:center;margin-bottom:10px;'>";
        html += "<label style='font-size:12px;font-weight:bold;color:#8e44ad;margin-left:20px;margin-right:10px;'>PULSADOR "+String(i+1)+"</label>";
        html += "<select name='modo_"+String(perfil)+"_"+String(i)+"'>";
        html += "<option value='PC'"+String(modoActual=="PC"?" selected":"")+">PC</option>";
        html += "<option value='CC'"+String(modoActual=="CC"?" selected":"")+">CC</option>";
        html += "<option value='RL'"+String(modoActual=="RL"?" selected":"")+">RL</option>";
        html += "</select>";
        html += "<input type='number' id='valor_"+String(perfil)+"_"+String(i)+"' name='valor_"+String(perfil)+"_"+String(i)+"' min='0' max='127' value='"+String(valor)+"' style='width:45px;margin-right:20px;"+String(modoActual=="RL"?"background:#ccc;' disabled":"'")+"'>";

        for(int r=0;r<NUM_RELES;r++){
            bool estado = EEPROM.read(ADDR_RELES(perfil)+(i*NUM_RELES)+r)==1;
            html += "<label style='font-size:0.8em;color:#fff;margin-right:10px;display:inline-flex;align-items:center;'>";
            html += "<input type='checkbox' name='r"+String(r+1)+"_"+String(perfil)+"_"+String(i)+"' value='1' "+(estado?"checked":"")+" style='accent-color:#8e44ad;margin-right:6px;'> Relé "+String(r+1)+"</label>";
        }
        html += "</div>";
    }

    for(int i=0;i<NUM_EXPRESION;i++){
        int valorExp = EEPROM.read(ADDR_CC_EXP(perfil)+i);
        html += "<div style='display:flex;align-items:center;margin-bottom:10px;'>";
        html += "<label style='font-size:12px;font-weight:bold;color:#8e44ad;margin-left:20px;margin-right:10px;'>CC PEDAL EXPRESIÓN " + String(i + 1) + "</label>";
        html += "<input type='number' name='EXP_" + String(perfil) + "_" + String(i) + "' min='0' max='127' value='" + String(valorExp) + "' style='width:45px;margin-left:12px;'>";
        html += "</div>";
    }

    int iniZero = EEPROM.read(ADDR_PC_CERO(perfil));
    html += "<div style='margin-left:20px;margin-bottom:20px;'>";
    html += "<label style='font-size:12px;color:#fff;display:inline-flex;align-items:center;'>";
    html += "<input type='checkbox' name='zero_" + String(perfil) + "' value='1' " + String(iniZero==1?"checked":"") + " style='accent-color:#8e44ad;margin-right:6px;'> Iniciar PC en 0 (mensaje MIDI PC-1)";
    html += "</label></div>";

    html += "</div>";
    return html;
}

String generarPerfilDefectoSelect(){
    EEPROM.begin(EEPROM_TOTAL + 16);
    int perfilDef = EEPROM.read(ADDR_PERFIL_DEFECTO);
    if(perfilDef<0 || perfilDef>=NUM_PERFILES) perfilDef=0;
    EEPROM.end();

    String html = "<label for='perfil_defecto' style='margin-left:20px;color:#fff;font-weight:bold;'>Perfil por defecto:</label>";
    html += "<select name='perfil_defecto' id='perfil_defecto'>";
    for(int p=0;p<NUM_PERFILES;p++){
        html += "<option value='"+String(p)+"'"+String(p==perfilDef?" selected":"")+">Perfil "+String(p+1)+"</option>";
    }
    html += "</select>";
    return html;
}

void iniciarServidorWeb(){
    WiFi.softAP(AP_SSID,AP_PASSWORD);
    WiFi.setHostname(HOST_NAME);

    server.on("/",HTTP_GET,[](AsyncWebServerRequest *request){
        String htmlFinal = String(paginaHTML);
        int perfilSel = EEPROM.read(ADDR_PERFIL_DEFECTO);
        htmlFinal.replace("%PERFILES%",generarPerfilesTabs(perfilSel));
        String contenidos="";
        for(int p=0;p<NUM_PERFILES;p++) contenidos += generarPulsadoresPorPerfil(p,perfilSel);
        htmlFinal.replace("%PULSADORES%",contenidos);
        htmlFinal.replace("%PERFIL_DEFECTO_SELECT%", generarPerfilDefectoSelect());
        request->send(200,"text/html; charset=UTF-8",htmlFinal);
    });

    server.on("/set",HTTP_POST,[](AsyncWebServerRequest *request){
        EEPROM.begin(EEPROM_TOTAL + 16);

        // Guardar perfil por defecto primero
        if(request->hasParam("perfil_defecto",true)){
            int pdef = request->getParam("perfil_defecto",true)->value().toInt();
            if(pdef<0) pdef=0;
            if(pdef>=NUM_PERFILES) pdef=NUM_PERFILES-1;
            EEPROM.write(ADDR_PERFIL_DEFECTO, pdef);
        }

        // Guardar pulsadores, expresion, relés y cero
        for(int perfil=0;perfil<NUM_PERFILES;perfil++){
            for(int i=0;i<NUM_PULSADORES;i++){
                String nameModo="modo_"+String(perfil)+"_"+String(i);
                String nameValor="valor_"+String(perfil)+"_"+String(i);
                if(!request->hasParam(nameModo,true) || !request->hasParam(nameValor,true)) continue;

                String modo = request->getParam(nameModo,true)->value();
                int valor = request->getParam(nameValor,true)->value().toInt();

                if(modo=="RL"){
                    EEPROM.write(ADDR_PULSADORES(perfil)+i,0);
                    EEPROM.write(ADDR_CC_PULS(perfil)+i,0);
                    EEPROM.write(ADDR_MODO_PULS(perfil)+i,2);
                } else if(modo=="PC"){
                    EEPROM.write(ADDR_PULSADORES(perfil)+i,valor);
                    EEPROM.write(ADDR_CC_PULS(perfil)+i,0);
                    EEPROM.write(ADDR_MODO_PULS(perfil)+i,0);
                } else if(modo=="CC"){
                    if(valor<0) valor=0;
                    if(valor>127) valor=127;
                    EEPROM.write(ADDR_CC_PULS(perfil)+i,valor);
                    EEPROM.write(ADDR_PULSADORES(perfil)+i,0);
                    EEPROM.write(ADDR_MODO_PULS(perfil)+i,1);
                }

                for(int r=0;r<NUM_RELES;r++){
                    String nameR="r"+String(r+1)+"_"+String(perfil)+"_"+String(i);
                    bool estado = request->hasParam(nameR,true);
                    EEPROM.write(ADDR_RELES(perfil)+(i*NUM_RELES)+r, estado?1:0);
                }
            }

            for(int e=0;e<NUM_EXPRESION;e++){
                String nameExp="EXP_"+String(perfil)+"_"+String(e);
                if(request->hasParam(nameExp,true)){
                    int valExp = request->getParam(nameExp,true)->value().toInt();
                    if(valExp<0) valExp=0;
                    if(valExp>127) valExp=127;
                    EEPROM.write(ADDR_CC_EXP(perfil)+e,valExp);
                }
            }

            String nameZero = "zero_" + String(perfil);
            EEPROM.write(ADDR_PC_CERO(perfil), request->hasParam(nameZero,true)?1:0);
        }

        EEPROM.commit();
        request->redirect("/");
    });

    server.begin();
}

void Modo_Configuracion(){
    EEPROM.begin(EEPROM_TOTAL + 16);
    for(int i=0;i<NUM_PULSADORES;i++){
        pinMode(PINES_LEDS[i],OUTPUT);
        digitalWrite(PINES_LEDS[i],LOW);
    }
    iniciarServidorWeb();
    while(true){
        ledsModoConfiguracion();
        delay(10);
    }
}

#endif
