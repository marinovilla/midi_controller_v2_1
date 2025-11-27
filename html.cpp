#include "html.h"

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
form { width: 100%; max-width: 430px; margin: 10px auto 40px auto; padding: 15px; background-color: #1e1e1e; border-radius: 8px; box-shadow: 0 4px 10px rgba(0,0,0,0.5); display: flex; flex-direction: column; }
label { font-size: 14px; color: #fff; margin-right: 10px; }
input[type='number'] { width: 80px; padding: 8px; margin: 5px; border: 2px solid #444; border-radius: 8px; font-size: 14px; background-color: #333; color: #fff; text-align: center; transition: all 0.3s ease; }
input[type='number']:focus { border-color: #8e44ad; outline: none; box-shadow: 0 0 8px rgba(142,68,173,0.8); }
select { padding: 8px; margin: 5px; border: 2px solid #444; border-radius: 8px; font-size: 14px; background-color: #333; color: #fff; transition: all 0.3s ease; }
select:focus { border-color: #8e44ad; outline: none; box-shadow: 0 0 8px rgba(142,68,173,0.8); }
input[type='submit'] { background-color: #8e44ad; color: white; border: none; padding: 12px 18px; font-size: 16px; border-radius: 6px; cursor: pointer; width: auto; margin-top: 10px; transition: background-color 0.3s; align-self: center; }
input[type='submit']:hover { background-color: #9b59b6; }
.container { text-align: center; padding: 10px; }
.tab-container { display: flex; justify-content: center; margin-bottom: 15px; }
.tab { padding: 6px 12px; margin: 0 3px; background-color: #333; cursor: pointer; border-radius: 4px; transition: background 0.3s; }
.tab.active { background-color: #8e44ad; }
.tabcontent { display: none; flex-direction: column; }
.tabcontent.active { display: flex; }
.pulsador { display: flex; align-items: center; justify-content: center; margin-bottom: 10px; }
.footer { text-align: center; padding: 5px; font-size: 10px; background-color: #333; color: white; position: fixed; width: 100%; bottom: 0; }
</style>
</head>
<body>
<div class='container'>
    <h1>CONFIGURACIÓN <br> MAPO MIDI CONTROLLER </h1>
    <div class='tab-container'>
        %PERFILES%
    </div>
    <form id="formConfig" action='/set' method='POST'>
        <input type="hidden" id="perfilSel" name="perfilSel" value="0">
        %PULSADORES%
        <div style="margin-top:10px;">
            <label style="font-size:12px;font-weight:bold;color:#fff;">Perfil por defecto:</label>
            <select name="perfil_defecto" style="margin-left:10px;">
                <option value="0">Perfil 1</option>
                <option value="1">Perfil 2</option>
                <option value="2">Perfil 3</option>
                <option value="3">Perfil 4</option>
                <option value="4">Perfil 5</option>
                <option value="5">Perfil 6</option>
            </select>
        </div>
        <input type='submit' value='Guardar'>
    </form>
</div>
<div class='footer'>
    <p>&copy; 2025 MAPO MIDI CONTROLLER</p>
</div>
<script>
function openTab(evt, tabId) {
    var i, tabcontent, tablinks;
    tabcontent = document.getElementsByClassName("tabcontent");
    for (i = 0; i < tabcontent.length; i++) tabcontent[i].classList.remove("active");
    tablinks = document.getElementsByClassName("tab");
    for (i = 0; i < tablinks.length; i++) tablinks[i].classList.remove("active");
    document.getElementById(tabId).classList.add("active");
    evt.currentTarget.classList.add("active");
}
function actualizarCampoValor(perfil, i){
    var modo=document.getElementsByName('modo_'+perfil+'_'+i)[0].value;
    var campo=document.getElementById('valor_'+perfil+'_'+i);
    var hiddenCampo=document.getElementById('hidden_valor_'+perfil+'_'+i);
    if(!hiddenCampo){
        hiddenCampo=document.createElement('input');
        hiddenCampo.type='hidden';
        hiddenCampo.id='hidden_valor_'+perfil+'_'+i;
        hiddenCampo.name='valor_'+perfil+'_'+i;
        document.getElementById('formConfig').appendChild(hiddenCampo);
    }
    if(modo==='RL'){
        campo.disabled=true;
        campo.style.background='#ccc';
        hiddenCampo.value=0;
        campo.value='';
    }else{
        campo.disabled=false;
        campo.style.background='';
        hiddenCampo.value=campo.value;
    }
}
document.getElementById('formConfig').addEventListener('submit', function(){
    for(var p=0;p<6;p++){
        for(var i=0;i<8;i++){
            var campo=document.getElementById('valor_'+p+'_'+i);
            var hiddenCampo=document.getElementById('hidden_valor_'+p+'_'+i);
            if(hiddenCampo) hiddenCampo.value=campo.disabled ? 0 : campo.value;
        }
    }
});
</script>
</body>
</html>
)rawliteral";
