+	Actualizar lista de repositorios
        sudo apt update
    ++ actualizar firmware
        sudo apt install firmware-brcm80211
    ++ Para asegurarse de tener las últimas actualizaciones, que podrían incluir controladores necesarios
        sudo apt update && sudo apt full-upgrade -y

+   Reiniciar la Raspberry Pi
    sudo reboot

+	Lista de dispositivos
	v4l2-ctl --list-devices
	++	me muestra al final de una larga lista, lo siguiente:
		USB Video: USB Video (usb-xhci-hcd.1-1):
			/dev/video0
			/dev/video1
			/dev/media2

+	FFmpeg y GStreamer
	++	para que FFmpeg muestre la imagen recibida
		ffmpeg -f v4l2 -i /dev/video0 -f x11grab -s 1920x1080 -pix_fmt yuv420p -r 30 -f sdl "Screen"
	++	para que GStreamer muestre la imagen recibida
		gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! autovideosink

+	 INSTALAR wpa_supplicant y dnsmasq
    sudo apt update
    sudo apt install wpasupplicant dnsmasq -y

+	NANO Configurar Wi-Fi Direct en la Raspberry Pi 5
        sudo nano /etc/wpa_supplicant/wpa_supplicant-p2p.conf
    ++	en el archivo:
        ctrl_interface=/var/run/wpa_supplicant
        update_config=1
        p2p_go_intent=15
        device_name=RaspberryPi5
    ++ verificar que los permisos sean correctos
        sudo chmod 600 /etc/wpa_supplicant/wpa_supplicant-p2p.conf
    ++ p2p_go_intent=15: Configura la Raspberry Pi 5 como "Group Owner" con la prioridad más alta
    ++ device_name=RaspberryPi5: Nombre del dispositivo Wi-Fi Direct

+   antes de iniciar wifi direct
    ++ hacer verificaciones
        ...
    ++ desconectar cualquier red Wi-Fi
        +++ si estás usando nmcli
            sudo nmcli dev disconnect wlan0
        +++ si no estás usando nmcli puedes directamente desactivar la conexión
            sudo ip link set wlan0 down
        +++ si estás usando dhcpcd
            sudo dhclient -r wlan0
        +++ para verificar que no está conectada a ninguna red( debe mostrar disconnected para wlan0)
            nmcli dev status
            ++ posible respuesta: Error: NetworkManager is not running
    ++ Eliminar cualquier proceso previo de wpa_supplicant
        sudo pkill wpa_supplicant
    ++ Eliminar archivos residuales
        sudo rm /var/run/wpa_supplicant/wlan0



+	para iniciar wifi direct con soporte P2P( wlan0 debe estar activa -state UP- y desbloqueada -rfkill unblock all- )
        sudo wpa_supplicant -Dnl80211 -iwlan0 -c/etc/wpa_supplicant/wpa_supplicant-p2p.conf -B
    ++ modo de depuración más detallado
        sudo wpa_supplicant -d -Dnl80211 -iwlan0 -c/etc/wpa_supplicant/wpa_supplicant-p2p.conf
    ++   verificar que está funcionando y en modo wifi direct
            sudo iw dev wlan0 info
            iw dev wlan0 info
        +++ se verá algo como:
            Interface wlan0
            Type: P2P-GO
        +++ Si dice P2P-GO, significa que la Raspberry Pi 5 está funcionando como el Group Owner para Wi-Fi Direct
    ++  resultados comunes¿ de error?
            ++ 1
                nl80211: kernel reports: Match already configured
                Failed to create interface p2p-dev-wlan0: -16 (Device or resource busy)
                nl80211: Failed to create a P2P Device interface p2p-dev-wlan0
                P2P: Failed to enable P2P Device interface
            ++ 2
                Successfully initialized wpa_supplicant
                nl80211: kernel reports: Registration to specific type not supported
        +++ Para solucionarlo
            ++ Detén dhcpcd
                    sudo systemctl stop dhcpcd
                +++ resultado común: Failed to restart dhcpcd.service: Unit dhcpcd.service not found
            ++ Detén NetworkManager
                sudo systemctl stop NetworkManager
            ++ verificar procesos que puedan interferir
                    sudo lsof /dev/wlan0
                +++ detenerlo con
                    sudo pkill <nombre_del_proceso>
                    sudo pkill wpa_supplicant
                +++ respuesta común¿ error?
                    lsof: WARNING: can't stat() fuse.gvfsd-fuse file system /run/user/1000/gvfs
                    Output information may be incomplete.
                    lsof: WARNING: can't stat() fuse.portal file system /run/user/1000/doc
                    Output information may be incomplete. 
                    lsof: status error on /dev/wlan0: NO such file or directory
            ++ limpiar y reiniciar la interfaz
                sudo ip link set wlan0 down
                sudo ip link set wlan0 up
            ++ Confirma que wlan0 soporte Wi-Fi Direct
                iw list | grep -A 10 "Supported interface modes"
+ verificar que la interfaz está activa
    ip link
    ++ Resultados comunes
        state DOWN      mode DEFAULT    group default
        state DOWN      mode DORMANT    group default
    ++ para activar la interfaz( state UP en lugar de state DOWN)
        sudo ip link set wlan0 up
        +++ o reiniciar
            sudo ip link set wlan0 down
            sudo ip link set wlan0 up
+   verificar controladores cargados. registro del kernel.
        sudo dmesg | grep wlan0
    ++ ¿ no muestra nada?
+   Verifica si el adaptador Wi-Fi está habilitado
        rfkill list
    ++ resultado común
        0: phy0: Wireless LAN
            Soft blocked: yes
            Hard blocked: no
        +++ si está bloqueado
            sudo rfkill unblock all
+   reiniciar servicios de red
    ++ Si estás usando NetworkManager
        sudo systemctl restart NetworkManager
    ++ Si estás usando dhcpcd
        sudo systemctl restart dhcpcd
            ++ ¿Failed to restart dhcpcd.service: Unit dhcpcd.service not found.?
+   verificar que el hardware wifi está presente
        lspci | grep -i network
+   diagnóstico de hardware
        lsusb
    ++ Busca un dispositivo relacionado con Broadcom o adaptadores Wi-Fi. Si no aparece, puede ser un problema físico

+   verificar que no hay procesos residuales
        ps aux | grep wpa_supplicant
    ++ eliminar cualquier instancia activa
        sudo kill <PID>
        ++ o detener todas las instancias activas
            sudo pkill wpa_supplicant
    ++ Si hay un servicio que está reiniciando otras instancias de wpa_supplicant
        ++ ver si hay algún servicio reiniciando
            sudo systemctl status wpa_supplicant
            +++ si aparece active(running), es este servicio el que está reiniciando wpa_supplicant
        ++ Detén y deshabilita wpa_supplicant.service
            sudo systemctl stop wpa_supplicant
            sudo systemctl disable wpa_supplicant
        ++ si hay otros servicios que estén reiniciando
            sudo systemctl stop NetworkManager
            sudo systemctl disable NetworkManager
        ++ en sistemas dhcpcd
            sudo systemctl stop dhcpcd
            sudo systemctl disable dhcpcd
        ++ Elimina el archivo de control residual
                sudo rm -f /var/run/wpa_supplicant/wlan0


+   Configura el grupo Wi-Fi Direct
    ++ crear el grupo wifi direct
            sudo iw phy0 interface add p2p-wlan0 type __p2p
        ++ activar esta nueva interfaz
            sudo ip link set p2p-wlan0 up
        ++ configurar dirección ip
            sudo ip addr add 192.168.5.1/24 dev p2p-wlan0
        ++ verificar configuración
            ip addr show p2p-wlan0
+   Configurar wifi direct manualmente
    ++ crear interfaz
        sudo iw dev wlan0 interface add p2p-wlan0 type __p2p
    ++ configurar interfaz
        sudo ip link set p2p-wlan0 up
    ++ habilitar wifi direct. Configura el modo Group Owner (GO)
        sudo iw dev p2p-wlan0 set type managed
        ++ ¿configurar modo wifi direct(P2P-GO)?
            sudo iw p2p-wlan0 set type managed
    ++ agregar ip
        sudo ip addr add 192.168.5.1/24 dev p2p-wlan0
    ++ verificar
        ip link show p2p-wlan0
        ip addr show p2p-wlan0
+   Si wpa_supplicant no configura el modo Group Owner automáticamente  ***
        sudo wpa_cli -i wlan0 p2p_group_add
    ++  Verifica el nuevo grupo creado
            sudo wpa_cli -i wlan0 p2p_group_list
            ip link
            ip addr show p2p-wlan0
        Si no tiene una dirección ip asignada
            sudo ip addr add 192.168.5.1/24 dev p2p-wlan0


Alternativa: Usa otro enfoque. Si wpa_cli no está cooperando, considera:Configurar un punto de acceso (AP)

    
    

+	Verifica la IP asignada(Toma nota de la IP, ya que será necesaria para transmitir el flujo de video)
ip addr show wlan0

+   Si la conexión falla, revisa los registros con 
        sudo journalctl -u wpa_supplicant
    ++ verificar la conectividad entre dispositivos
        ping

+	conectar Raspberry Pi 4
	++	buscar redes. Verifica conectividad con otra Raspberry Pi
		sudo iw dev wlan0 scan | grep SSID
	++	conectar
		sudo iw dev wlan0 connect <SSID>
		sudo iw dev wlan0 connect RaspberryPi5
	++	para confirmar la conexión
		ip addr show wlan0

+	verificar que tengo una versión compatible de SO
	lsb_release -a
	++	respuesta Raspberry Pi 5
		No LSB modules are available.
		Distribuitor ID: Debian
		Description Debian GNU/Linux 12 (bookworm)
		Release: 12
		Codename: bookworm

+	sistema alternativo a wpa_supplicant: hostapd 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Reactivar ethernet
sudo systemctl enable NetworkManager
sudo systemctl start NetworkManager

sudo systemctl status NetworkManager
Si el estado no está active(running): sudo systemctl restart NetworkManager

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ AP

Lista las conexiones disponibles
    nmcli connection show
Crea una conexión nueva para wlan0
    sudo nmcli connection add type wifi ifname wlan0 con-name MyAccessPoint autoconnect no ssid MyRaspberryPiAP
Configura una IP estática para el Access Point
sudo nmcli connection modify MyAccessPoint ipv4.addresses 192.168.50.1/24
sudo nmcli connection modify MyAccessPoint ipv4.method manual
sudo nmcli connection modify MyAccessPoint wifi.mode ap             
sudo nmcli connection modify MyAccessPoint wifi.band bg
sudo nmcli connection modify MyAccessPoint wifi-sec.key-mgmt wpa-psk
sudo nmcli connection modify MyAccessPoint wifi-sec.psk "securepassword123"

Verificar conexión
    sudo nmcli connection show MyAccessPoint
Activa la conexión del Access Point
    sudo nmcli connection up MyAccessPoint
Revisa el estado de wlan0
    nmcli device status
    //Deberías ver que wlan0 está en modo connected y que está usando la conexión MyAccessPoint
Confirma que wlan0 tiene la IP estática asignada
    ip addr show wlan0

Conectar desde Raspberry Pi 4
asigna una IP manualmente
    sudo ip addr add 192.168.50.2/24 dev wlan0
Prueba la conectividad con un ping a la Raspberry Pi 5
    ping 192.168.50.1

+++ Transmisión de video desde la Raspberry Pi 5 al dispositivo conectado

Raspberry Pi 4: 192.168.50.29/24

Medición de latencia
    cronometro cel
        ffmpeg -f v4l2 -i /dev/video0 -f x11grab -s 1920x1080 -pix_fmt yuv420p -r 30 -f sdl "Screen"
        0,99
        1,20
        0,94
        1,06
        1,06
        ffmpeg -f v4l2 -framerate 30 -i /dev/video0 -pix_fmt yuv420p -video_size 1920x1080 -fflags nobuffer -flags low_delay -f sdl "Screen"
        1,26
        1,33
        1,40
        1,26
        1,39
        ffmpeg -f v4l2 -framerate 30 -i /dev/video0 -pix_fmt yuv420p -video_size 1280x720 -fflags nobuffer -flags low_delay -f sdl "Screen"
        0,94
        1,00
        1,07
        1,00
        1,00
        ffmpeg -f v4l2 -rtbufsize 32M -framerate 30 -i /dev/video0 -pix_fmt yuv420p -video_size 1920x1080 -fflags nobuffer -flags low_delay -f sdl "Screen"
        1,00
        0,73
        1,00
        0,74
        0,87
        ffmpeg -f v4l2 -rtbufsize 32M -framerate 30 -i /dev/video0 -pix_fmt yuv420p -video_size 1920x1080 -fflags nobuffer -flags low_delay -f sdl "Screen"
        1,00
        1,27
        1,07
        0,8
        1,26
    video grabación
        ...

comandos
-r 30: Configura la velocidad de fotogramas de salida (output)
-framerate 30: Configura la velocidad de fotogramas de entrada (input)
-s 1920x1080: Establece el tamaño de video para la salida (output)
-video_size 1920x1080: Establece el tamaño de video para la entrada (input)
Cuando capturas desde un dispositivo como /dev/video0, usa -video_size

Orden sugerido
    Parámetros de ENTRADA: -f, -framerate, -video_size, -i, -input_format
        Opciones globales: -threads, -loglevel
        Opciones de procesamiento: -pix_fmt, -vcodec, -b:v, etc
    Configuración de SALIDA: -f, -r, -s, output_file, 
Codificación:
    Video: -vcodec libx264 -preset ultrafast.
    Audio: -acodec aac
Búfer:
    Entrada: -fflags nobuffer.
    Salida: -b:v 2M (bitrate de video)
Transmisión:
    Configuración para UDP: -f mpegts udp://192.168.50.2:1234
-f sdl "Screen": se mostrará en una ventana con SDL (Simple DirectMedia Layer)


-input_format mjpeg
-input_format yuyv422
-pix_fmt yuyv422
-pix_fmt yuv420p es opcional si el visor requiere un formato compatible


ffmpeg -f v4l2 -input_format mjpeg -video_size 1920x1080 -framerate 30 -i /dev/video0 -f sdl "Ale"
innecesario: -pix_fmt yuyv422 -s 1920x1080 -r 30
ffmpeg -f v4l2 -input_format mjpeg -video_size 1920x1080 -framerate 30 -i /dev/video0 -pix_fmt yuv420p -f sdl "Ale"

ffmpeg -f v4l2 -rtbufsize 32M -framerate 30 -i /dev/video0 -pix_fmt yuv420p -video_size 1920x1080 -fflags nobuffer -flags low_delay -f sdl "Screen"

ffmpeg -f v4l2 -input_format mjpeg -video_size 1920x1080 -framerate 30 -i /dev/video0 -pix_fmt yuv420p -f sdl "Ale"

+++ Conectar la 4
ip Raspberry Pi 4: 192.168.50.29/24

ffmpeg -f v4l2 -i /dev/video0 -pix_fmt yuv420p -f mpegts udp://192.168.50.2:1234
ffmpeg -f v4l2 -i /dev/video0 -s 640x480 -b:v 1M -f mpegts udp://192.168.50.2:1234
ffmpeg -f v4l2 -input_format mjpeg -framerate 30 -i /dev/video0 -f mpegts udp://127.0.0.1:1234

ping 192.168.50.1
vlc udp://@:1234
mpv udp://0.0.0.0:1234



framerate 15, ffplay nobuffer
1
UDP buffer, codec libx264
ffmpeg -f v4l2 -input_format mjpeg -framerate 15 -i /dev/video0 -c:v libx264 -f mpegts udp://192.168.50.29:1234?udp_buffer_size=65536
ffplay -fflags nobuffer udp://0.0.0.0:1234
2
RTP, codec libx264
ffmpeg -f v4l2 -input_format mjpeg -framerate 15 -i /dev/video0 -c:v libx264 -f rtp rtp://192.168.50.29:1234
ffplay -fflags nobuffer rtp://0.0.0.0:1234
3
UDP buffer, RAW Video
ffmpeg -f v4l2 -input_format mjpeg -framerate 15 -i /dev/video0 -f rawvideo udp://192.168.50.29:1234?udp_buffer_size=65536
ffplay -fflags nobuffer udp://0.0.0.0:1234


*********************
+ La Raspberry Pi 5 está enviando con:
ffmpeg -f v4l2 -input_format mjpeg -framerate 10 -video_size 1920x1080 -i /dev/video0 -f mjpeg udp://192.168.50.29:1234 
ffmpeg -f alsa -ar 44100 -ac 2 -i default -f mpegts udp://192.168.50.29:1235
+ La Raspberry Pi 4 está recibiendo con:
ffmpeg -f mjpeg -i udp://0.0.0.0:1234 -pix_fmt yuv420p -f sdl "Video"
en otra terminal:
wmctrl -r "Video" -b add,fullscreen


para pantalla completa:
xrandr --output HDMI-1 --mode 1920x1080
ffmpeg -f mjpeg -i udp://0.0.0.0:1234 -pix_fmt yuv420p -f sdl "Video"
**********************

