# === Raspberry Pi backend.py med testfunktioner og dynamisk målingskontrol ===

from flask import Flask, render_template
from flask_socketio import SocketIO, emit
import serial
import eventlet

# Gør det muligt at bruge eventlet sammen med Flask (for async UART håndtering)
eventlet.monkey_patch()

# Opret Flask-app og SocketIO server
app = Flask(__name__)
socketio = SocketIO(app)

# Forsøg at åbne UART-forbindelse til MEGA
try:
    ser = serial.Serial('/dev/serial0', 9600)
    uart_connected = True
    print("UART forbindelse oprettet via /dev/serial0.")
except Exception as e:
    print("UART ikke tilgængelig – MEGA ikke tilsluttet:", e)
    uart_connected = False

# Rute til forsiden (index.html)
@app.route('/')
def index():
    return render_template('index.html')

# Dictionary til at holde styr på hvilke sensorer der er aktive
active_sensors = {
    'TEMP': True,
    'LIGHT': True,
    'CO2': True,
    'HUMIDITY': True,
    'WINDOW': True
}

# Flag for om kontinuerlig datastrøm er aktiveret eller ej
data_stream_enabled = True

# Modtag kontrolbeskeder fra frontend
@socketio.on('control')
def handle_control(data):
    global data_stream_enabled

    # Hvis en test anmodes fra teknikermenuen
    if 'test' in data:
        test_type = data['test']
        if uart_connected:
            # Send en testbesked afhængig af typen
            if test_type == 'TEMP':
                ser.write(b'TEMP:TEST\n')
            elif test_type == 'LIGHT':
                ser.write(b'LIGHT:TEST\n')
            elif test_type == 'CO2':
                ser.write(b'CO2?\n')
            elif test_type == 'HUMIDITY':
                ser.write(b'HUMIDITY?\n')
            elif test_type == 'WINDOW':
                ser.write(b'OPEN\n')
                ser.write(b'CLOSE\n')
        emit('feedback', {'status': f'Test sendt til {test_type}'})
        return

    # Håndter almindelige kontrolkommandoer fra brugerinterfacet
    if 'window' in data and active_sensors['WINDOW']:
        cmd = 'OPEN' if data['window'] else 'CLOSE'
        ser.write((cmd + '\n').encode())

    if 'temperature' in data and active_sensors['TEMP']:
        temp_cmd = f"TEMP:{data['temperature']}\n"
        ser.write(temp_cmd.encode())

    if 'light' in data and active_sensors['LIGHT']:
        light_cmd = f"LIGHT:{data['light']}\n"
        ser.write(light_cmd.encode())

    emit('feedback', {'status': 'Sendt'})

# Baggrundsopgave der læser data løbende fra MEGA og sender til frontend
def uart_background_read():
    global data_stream_enabled
    while uart_connected:
        if ser.in_waiting:
            uart_reading = ser.readline().decode('utf-8').strip()

            # Hvis datastrøm er slået fra, ignorer input
            if not data_stream_enabled:
                continue

            # Parse og videresend data afhængigt af typen
            if uart_reading.startswith("TEMP:") and active_sensors['TEMP']:
                socketio.emit('temp_update', {'value': uart_reading[5:]})
            elif uart_reading.startswith("LIGHT:") and active_sensors['LIGHT']:
                socketio.emit('light_update', {'value': uart_reading[6:]})
            elif uart_reading.startswith("CO2:") and active_sensors['CO2']:
                socketio.emit('co2_update', {'value': uart_reading[4:]})
            elif uart_reading.startswith("HUMIDITY:") and active_sensors['HUMIDITY']:
                socketio.emit('humidity_update', {'value': uart_reading[9:]})
            else:
                # Ukendt eller generel besked videresendes til teknikermenu
                socketio.emit('uart_response', {'response': uart_reading})

        eventlet.sleep(0.5)  # Lidt pause så CPU ikke stresses

# Start baggrundsopgave hvis UART er aktiv
if uart_connected:
    socketio.start_background_task(uart_background_read)

# Start Flask-server
if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000)
