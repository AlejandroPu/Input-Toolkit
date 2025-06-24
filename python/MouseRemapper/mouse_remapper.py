# -*- coding: utf-8 -*-
"""
Created on Fri Sep 13 18:43:33 2024

@author: retac
lo mismo que el primer código,
2 además, se inhabilita el mouse mientras se ejecuta el código
3 eliminación de posición fija, utilización de scroll para mover el mouse
4 cambiar estado de supress con botón medio
"""

from pynput import mouse, keyboard
import threading
import time

print("Para terminar el script, presione ESC.")

running = True  # Variable para controlar la ejecución
last_cursor_position = None  # Almacena la última posición del cursor
last_scroll_time = None  # Almacena el tiempo del último evento de desplazamiento
factor = 1  # Valor inicial del factor
initial_factor = 1  # Almacenar el valor inicial para restablecer
max_factor = 50  # Valor máximo del factor

# Variables para detectar doble clic con el botón derecho
last_right_click_time = None  # Almacena el tiempo del último clic derecho
double_click_threshold = 0.5  # Tiempo máximo entre clics para considerar doble clic

# Variable para controlar el modo de desplazamiento (eje 'x' o 'y')
scroll_mode = 'y'  # Por defecto, mover en el eje 'y'

# Variable para controlar el estado de suppress
suppress_events = True  # Estado inicial de suppress

# Controlador de mouse
mouse_controller = mouse.Controller()

# Variable para almacenar el listener del mouse
mouse_listener = None

# Bloqueo para sincronización
listener_lock = threading.Lock()

# Función que se ejecuta al presionar un botón del mouse
def on_click(x, y, button, pressed):
    global running, last_right_click_time, scroll_mode, suppress_events, mouse_listener
    if not running:
        return False  # Detener el listener de mouse inmediatamente

    # Solo nos interesa cuando se presiona el botón, no cuando se libera
    if pressed:
        if button == mouse.Button.right:
            current_time = time.time()
            if last_right_click_time is None:
                last_right_click_time = current_time
            else:
                time_diff = current_time - last_right_click_time
                if time_diff <= double_click_threshold:
                    # Se ha detectado un doble clic con el botón derecho
                    # Cambiar el modo de desplazamiento
                    if scroll_mode == 'y':
                        scroll_mode = 'x'
                        print("Modo de desplazamiento cambiado al eje 'x'")
                    else:
                        scroll_mode = 'y'
                        print("Modo de desplazamiento cambiado al eje 'y'")
                    # Reiniciar last_right_click_time para evitar múltiples cambios
                    last_right_click_time = None
                else:
                    # No es un doble clic, actualizar last_right_click_time
                    last_right_click_time = current_time
        elif button == mouse.Button.middle:
            # Al hacer clic en el botón del medio, cambiar suppress
            suppress_events = not suppress_events
            print(f"Suppress cambiado a {suppress_events}")
            # Reiniciar el listener del mouse con el nuevo valor de suppress en un hilo separado
            threading.Thread(target=restart_mouse_listener).start()
        else:
            # Si se presiona otro botón, reiniciar last_right_click_time
            last_right_click_time = None

    action = 'presionado' if pressed else 'liberado'
    print(f"Botón {button} {action} en ({x}, {y})")

# Función que se ejecuta al mover el mouse
def on_move(x, y):
    if not running:
        return False  # Detener el listener de mouse inmediatamente
    # Puedes agregar lógica aquí si es necesario

# Función que se ejecuta al desplazar la rueda del mouse
def on_scroll(x, y, dx, dy):
    global last_cursor_position, last_scroll_time, factor
    if not running:
        return False  # Detener el listener de mouse inmediatamente

    # Obtener el tiempo actual
    current_time = time.time()

    if last_scroll_time is None:
        last_scroll_time = current_time
    else:
        time_diff = current_time - last_scroll_time
        if time_diff < 0.3:
            factor *= 1.3
            factor = min(factor, max_factor)
        else:
            factor = initial_factor
        last_scroll_time = current_time

    # Mover el cursor basado en el modo actual
    current_x, current_y = mouse_controller.position

    if scroll_mode == 'y':
        new_x = current_x
        new_y = current_y - dy * factor
    else:
        new_x = current_x - dy * factor
        new_y = current_y

    mouse_controller.position = (new_x, new_y)
    last_cursor_position = (new_x, new_y)

    print(f"Rueda movida en dy={dy}, factor={factor}, cursor movido a ({new_x}, {new_y})")

# Función que se ejecuta al presionar una tecla
def on_press(key):
    global running
    if key == keyboard.Key.esc:
        print("Se ha presionado 'Esc'. Deteniendo el script...")
        running = False
        # Detener los listeners
        stop_listeners()
        return False

# Función para reiniciar el listener del mouse
def restart_mouse_listener():
    global mouse_listener
    with listener_lock:
        if mouse_listener is not None:
            mouse_listener.stop()
            mouse_listener.join()

        mouse_listener = mouse.Listener(on_click=on_click, on_move=on_move,
                                        on_scroll=on_scroll, suppress=suppress_events)
        mouse_listener.start()

# Función para detener los listeners
def stop_listeners():
    global mouse_listener
    with listener_lock:
        if mouse_listener is not None:
            mouse_listener.stop()
            mouse_listener.join()

# Función para iniciar los listeners
def start_listeners():
    global mouse_listener
    # Iniciar el listener del teclado
    keyboard_listener = keyboard.Listener(on_press=on_press)
    keyboard_listener.start()

    # Iniciar el listener del mouse
    restart_mouse_listener()

    # Esperar a que el listener del teclado termine
    keyboard_listener.join()
    # Asegurarse de que el listener del mouse también termine
    with listener_lock:
        if mouse_listener is not None:
            mouse_listener.join()

if __name__ == "__main__":
    # Iniciar el proceso de escucha
    start_listeners()
