Project ini dibuat sebagai tugas akhir dengan fokus smart garden
# Tujuan Penelitian
Bertujuan untuk merancang, membuat, mengimplementasi sistem otomasi penyiraman air dan pemupukan cair di dalam mini greenhouse berbasis IoT (Internet of Things).

## Deskripsi singkat

Jadwal penyemprotan pupuk cair menggunakan sensor RTC DS3231, kendali aktuator berdasarkan kelembapan tanah yang menggunakan sensor capacitive soil moisture v1.2, pemrosesan data sensor dilakukan mikrokontroler ESP32. Paket data dikirim menggunakan protocol MQTT ke node-Red Dashboard berbasis web.

Pemantauan dan kendali set point dilakukan pada web. node-RED di-hosting di server dengan nginx sebagai reverse proxy ke domain iot.depermana.com.

## Metode Penelitian
Menggunakan metode rekayasa teknik dengan pengambilan data secara observasi laboratorium. Dimulai dari perancangan, pembuatan, dan pengujian. 

Penerapan penyiraman air menggunakan pendekatan metode metode Blaney-Criddle sebagai perhitungan teoritis kebutuhan air tanaman dengan data iklim BMKG di daerah penelitian. Data tersebut dijadikan acuan kebutuhan penyiraman air

## Perancangan
### blok diagram
<img src="https://i.imgur.com/uQvTJUeh.jpg">

### skematik
![Imgur](https://i.imgur.com/Ys81KyEh.png)

### pcb shield
![Imgur](https://i.imgur.com/SRJKY6lh.png)

### miniatur green house
![Imgur](https://i.imgur.com/dMfNJX2.png)

# Fitur Utama
- Penyiraman otomatis berdasarkan data kelembaban tanah.
- Pemupukan otomatis dengan jadwal yang dapat disesuaikan.
- water level tangki air dan pupuk cair
- suhu dan kelembapan udara GH
- Monitoring kondisi tanaman secara real-time.
- Kendali jarak jauh melalui web.
- data logger menggunakan time series database influx-db

## Teknologi yang Digunakan
- Mikrokontroller ESP32.
- Protokol MQTT broker mosquitto.
- Sensor kelembaban tanah capacitive soil moisture v1.2.
- RTC DS3231
- Sensor ultrasonik HC-SR04
- Sensor suhu dan kelembapan udara DHT 33
- Platform IoT (server running ubuntu, nginx webserver, node-Red, tld, lets encrypt).

# Tantangan Utama
- satu sensor kelembapan tanah tidak dapat merepresentasi keseluruhan nilai kelembapan tanah. Solusi menggunakan lebih dari 1 sensor kelembapan tanah dibeberapa titik tanah dan nilai terseut dirata-rata
- Keamanan sistem IoT untuk melindungi data dan perangkat.


# Hasil Penelitian
Proyek ini berhasil menghasilkan sistem yang dapat memberikan penyiraman dan pemupukan otomatis untuk tanaman berdasarkan kondisi kelembapan tanah dan jadwal pemupukan cair yang dapat ditentukan pengguna. Sistem ini juga memberikan akses jauh melalui internet untuk pengawasan dan kendali.

![Imgur](https://i.imgur.com/LykFGOJh.jpg)

![Imgur](https://i.imgur.com/jk6SZfYh.jpg) 

![Imgur](https://i.imgur.com/MKvohnLh.jpg)

![Imgur](https://i.imgur.com/CwmyEwyh.jpg)