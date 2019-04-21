#!/bin/bash
openssl enc -d -des -md md5 -k ./P02003_KEY -in fw_DWR-966_v1.0.0.6-relay_P02003.img.cpt -out fw_DWR-966_v1.0.0.6-relay_P02003.img
openssl enc -d -des -md md5 -k 97331724P02003 -in fw_DWR-966_v1.0.3CP_r03_P02003.img.cpt -out fw_DWR-966_v1.0.3CP_r03_P02003.img
openssl enc -d -des -md md5 -k 97331724P02003 -in fw_DWR-966_v1.0.0.7_P02003.img.cpt -out fw_DWR-966_v1.0.0.7_P02003.img
openssl enc -d -des -md md5 -k 97331724P02003 -in fw_DWR966_v1.0.0.8_r03_P02003.img.cpt -out fw_DWR966_v1.0.0.8_r03_P02003.img
openssl enc -d -des -md md5 -k 97331724P02003 -in fw_DWR966_v1.0.0.9_r05_P02003.img.cpt -out fw_DWR966_v1.0.0.9_r05_P02003.img
openssl enc -d -des -md md5 -k 97331724P02003 -in DWR-966_v3.0.0.2_r04_P02003.img.cpt -out DWR-966_v3.0.0.2_r04_P02003.img
