#ifndef HTML_H
#define HTML_H

#include <Arduino.h>

extern const char paginaHTML[];

const char paginaHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MAPO MIDI CONTROL</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #121212; color: #fff; margin: 0; padding: 0; }
        h1 { text-align: center; color: #8e44ad; font-size: 22px; margin-top: 20px; text-transform: uppercase; }
        form { width: 100%; max-width: 430px; margin: auto; padding: 15px; background-color: #1e1e1e; border-radius: 8px; 
               box-shadow: 0 4px 10px rgba(0, 0, 0, 0.5); display: flex; flex-direction: column; }
        label { font-size: 14px; color: #fff; margin-right: 10px; }
        input[type='number'] { width: 80px; padding: 8px; margin: 5px; border: 2px solid #444; border-radius: 8px; 
                               font-size: 14px; background-color: #333; color: #fff; text-align: center; transition: all 0.3s ease; }
        input[type='number']:focus { border-color: #8e44ad; outline: none; box-shadow: 0 0 8px rgba(142, 68, 173, 0.8); }
        select { padding: 8px; margin: 5px; border: 2px solid #444; border-radius: 8px; 
                 font-size: 14px; background-color: #333; color: #fff; transition: all 0.3s ease; }
        select:focus { border-color: #8e44ad; outline: none; box-shadow: 0 0 8px rgba(142, 68, 173, 0.8); }
        input[type='submit'] { background-color: #8e44ad; color: white; border: none; padding: 12px 18px; font-size: 16px; 
                               border-radius: 6px; cursor: pointer; width: auto; margin-top: 10px; transition: background-color 0.3s; align-self: center; }
        input[type='submit']:hover { background-color: #9b59b6; }
        .container { text-align: center; padding: 10px; }
        .pulsador { display: flex; align-items: center; justify-content: center; margin-bottom: 10px; }
        .footer { text-align: center; padding: 5px; font-size: 10px; background-color: #333; color: white; position: fixed; 
                  width: 100%; bottom: 0; }
    </style>
</head>
<body>

    <div class='container'>
        <h1>CONFIGURACIÓN <br> MAPO MIDI CONTROLLER </h1>
        <form action='/set' method='GET'>
            %PULSADORES%
            <input type='submit' value='Guardar'>
        </form>
    </div>

    <div class='footer'>
        <p>&copy; 2025 MAPO MIDI CONTROLLER</p>
    </div>

</body>
</html>
)rawliteral";

#endif
