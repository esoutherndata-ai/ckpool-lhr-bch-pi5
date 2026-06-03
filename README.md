# ckpool-lhr-bch-pi5

**Fixed ckpool fork for Bitcoin Cash (BCH) on Raspberry Pi 5**

This repository contains fixes and improvements for running a BCH mining pool on a Raspberry Pi 5.

---

## Problems Fixed

- SegWit `getblocktemplate` error
- BCH CashAddr (`q...`) address validation
- Build issues on ARM64 / Pi 5
- Systemd auto-start configuration

---

## How to Build on Pi 5

```bash
cd ckpool-lhr-bch
mkdir -p m4
./autogen.sh
./configure --prefix=/usr/local
make -j4
sudo make install

##CONFIGURATION (IMPORTANT)

cp ckpool-lhr-bch.conf.example ckpool-lhr-bch.conf
nano ckpool-lhr-bch.conf

##CURRENT CONFIGURATION
{
"btcd" : [
	{
		"url" : "127.0.0.1:8334",
		"auth" : "esoutherndata",
		"pass" : "YOUR_RPC_PASSWORD_HERE",
		"notify" : true
	}
],
"btcaddress" : "qzdua8nafl9l8qvp62u0xpnp9xn8uyq74svmdh6l3g",
"serverurl" : ["0.0.0.0:3334"],
"logdir" : "/media/esoutherndata/BCH-DATA/ckpool-logs"
}

##AUTO-START ON BOOT

sudo nano /etc/systemd/system/ckpool.service

##Use this service file

[Unit]
Description=ckpool BCH Mining Pool
After=bitcoind.service
Wants=bitcoind.service

[Service]
Type=simple
User=esoutherndata
WorkingDirectory=/home/esoutherndata/ckpool-lhr-bch
ExecStart=/usr/local/bin/ckpool -c /home/esoutherndata/ckpool-lhr-bch/ckpool-lhr-bch.conf
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target

##Then enable it

sudo systemctl daemon-reload
sudo systemctl enable ckpool.service
sudo systemctl start ckpool.service

##This README explains the problems we ran into and how we fixed them. It's clear and helpful for other users.
