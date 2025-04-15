from flask import Flask, render_template
from flask_socketio import SocketIO, emit
import serial
import eventlet
import time

eventlet.monkey_patch()

app = Flask(__name__)
socketio = SocketIO(app)

# === Prøv at oprette UART med det samme ===
try:
    print("[Init] Forsøger at åbne UART...")
    ser = serial.Serial('/dev/serial0', 9600, timeout=2)
    uart_connected = True
    print("✅ UART-forbindelse åbnet.")
except Exception as e:
    print(f"❌ Kunne ikke åbne UART: {e}")
    uart_connected = False

@app.route('/')
def index():
    print("[Route] Indlæser index.html")
    return render_template('index.html')

active_sensors = {
    'TEMP': True,
    'LIGHT': True,
    'CO2': True,
    'HUMIDITY': True,
    'WINDOW': True
}
data_stream_enabled = True

@socketio.on('control')
def handle_control(data):
    print(f"[Control] Modtog: {data}")
    global data_stream_enabled

    if not uart_connected:
        emit('feedback', {'status': 'UART ikke tilgængelig'})
        return

    if 'test' in data:
        emit('feedback', {'status': f"Test ikke understøttet i kompakt kommandomode"})
        return

    if 'toggle_stream' in data:
        data_stream_enabled = not data_stream_enabled
        print(f"[Stream Toggle] Datastrøm er nu: {'ON' if data_stream_enabled else 'OFF'}")
        status = "AKTIV" if data_stream_enabled else "STOPPET"
        emit('uart_response', {'response': f"Datastrøm er nu: {status}"})
        return

    # === Kompakt kommandomode ===
    command = None

    if 'window' in data and active_sensors['WINDOW']:
        if data['window'] == 'AUTO':
            command = 200
            print("[UART] Vindue: AUTO (200)")
        elif data['window'] == 'TOGGLE':
            command = 100
            print("[UART] Vindue: TOGGLE (100)")

    if 'temperature' in data and active_sensors['TEMP']:
        try:
            temp_val = int(float(data['temperature']))
            if 0 <= temp_val <= 99:
                command = 300 + temp_val
                print(f"[UART] Temperatur sat til: {temp_val} → Kommando: {command}")
                emit('auto_mode_update', {'sensor': 'TEMP', 'mode': 'MANUAL'})
        except:
            pass

    if 'light' in data and active_sensors['LIGHT']:
        if data['light'] == 'AUTO':
            command = 400
            print("[UART] Lys: AUTO (400)")
            emit('auto_mode_update', {'sensor': 'LIGHT', 'mode': 'AUTO'})
        else:
            try:
                light_val = int(float(data['light']))
                if light_val > 99:
                    light_val = 99
                if 0 <= light_val <= 99:
                    command = 500 + light_val
                    print(f"[UART] Lys sat til: {light_val}% → Kommando: {command}")
                    emit('auto_mode_update', {'sensor': 'LIGHT', 'mode': 'MANUAL'})
            except:
                pass

    if command is not None:
        print(f"[UART SEND] Sender kommando: {command}")
        ser.write(f"{command}\n".encode())
        print(f"[UART DEBUG] Kommando sendt til MEGA: {command}")
        emit('feedback', {'status': f'Sendt kommando: {command}'})
    else:
        emit('feedback', {'status': 'Ingen gyldig kommando identificeret'})

def uart_background_read():
    print("[UART Thread] Startet baggrundslæser...")
    buffer = ""

    while uart_connected:
        if ser.in_waiting:
            try:
                raw_bytes = ser.read(ser.in_waiting)
                decoded = raw_bytes.decode('utf-8', errors='ignore')
                buffer += decoded
                print(decoded.strip())

                lines = buffer.split('\n')
                buffer = lines[-1]  # gem ufuldstændig del

                for line in lines[:-1]:
                    line = line.strip()
                    if not line:
                        continue
                    print(line)
                    socketio.emit('uart_response', {'response': line})

            except Exception as e:
                print(f"❌ UART fejl: {e}")

        eventlet.sleep(0.5)

if uart_connected:
    print("[Main] Starter UART baggrundstråd...")
    socketio.start_background_task(uart_background_read)

if __name__ == '__main__':
    print("[Main] Starter Flask server...")
    socketio.run(app, host='0.0.0.0', port=5000)
