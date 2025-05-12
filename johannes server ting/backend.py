from flask import Flask, render_template
from flask_socketio import SocketIO, emit
import serial
import eventlet
import time
import threading

eventlet.monkey_patch()

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*", async_mode="eventlet")

# === Opret UART-forbindelse ===
try:
    print("[Init] Forsøger at åbne UART...")
    ser = serial.Serial('/dev/serial0', 9600, timeout=2)
    uart_connected = True
    print("✅ UART-forbindelse åbnet.")
except Exception as e:
    print(f"❌ Kunne ikke åbne UART: {e}")
    uart_connected = False

serial_lock = threading.Lock()

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
    global data_stream_enabled
    print(f"[Control] Modtog: {data}")

    if not uart_connected:
        emit('feedback', {'status': 'UART ikke tilgængelig'})
        return

    command = None

    if 'test' in data and data['test'] == "ALL":
        command = 600
        print("[Control] Systemtest anmodet → Kommando: 600")


    if 'window' in data and active_sensors['WINDOW']:
        if data['window'] == 'AUTO':
            command = 200
            emit('auto_mode_update', {'sensor': 'WINDOW', 'mode': 'AUTO'})
        elif data['window'] == 'TOGGLE':
            command = 100
            emit('auto_mode_update', {'sensor': 'WINDOW', 'mode': 'MANUAL'})

    if 'temperature' in data and active_sensors['TEMP']:
        try:
            temp_val = int(float(data['temperature']))
            if 0 <= temp_val <= 99:
                command = 300 + temp_val
                emit('auto_mode_update', {'sensor': 'TEMP', 'mode': 'MANUAL'})
        except:
            pass

    if 'light' in data and active_sensors['LIGHT']:
        if data['light'] == 'AUTO':
            command = 400
            emit('auto_mode_update', {'sensor': 'LIGHT', 'mode': 'AUTO'})
        else:
            try:
                light_val = int(float(data['light']))
                if light_val > 99:
                    light_val = 99
                if 0 <= light_val <= 99:
                    command = 500 + light_val
                    emit('auto_mode_update', {'sensor': 'LIGHT', 'mode': 'MANUAL'})
            except:
                pass

    if command is not None:
        try:
            with serial_lock:
                ser.write(f"{command}\n".encode())
            print(f"[UART SEND] Kommando sendt: {command}")
            emit('feedback', {'status': f'Sendt kommando: {command}'})
        except Exception as e:
            print(f"❌ UART fejl ved send: {e}")
            emit('feedback', {'status': 'Fejl ved afsendelse'})
    else:
        emit('feedback', {'status': 'Ingen gyldig kommando'})

def uart_background_read():
    print("[UART Thread] Startet baggrundslæser...")
    buffer = ""

    while uart_connected:
        try:
            if ser.in_waiting:
                with serial_lock:
                    raw_bytes = ser.read(ser.in_waiting)
                decoded = raw_bytes.decode('utf-8', errors='ignore')
                buffer += decoded

                lines = buffer.split('\n')
                buffer = lines[-1]

                for line in lines[:-1]:
                    line = line.strip()
                    if not line:
                        continue

                    if not data_stream_enabled:
                        continue

                    print(f"[UART] {line}")
                    socketio.emit('uart_response', {'response': line})
        except Exception as e:
            print(f"❌ Fejl i UART-læsning: {e}")

        eventlet.sleep(0.5)

if uart_connected:
    socketio.start_background_task(uart_background_read)

if __name__ == '__main__':
    print("[Main] Starter Flask server...")
    socketio.run(app, host='0.0.0.0', port=5000)

