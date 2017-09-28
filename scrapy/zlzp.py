#coding:utf-8
import requests
import time

url = 'http://weixin.zhaopin.com/UserCenter/StateQueryCarBon.aspx?cid='


for i in range(90000,99999):
    uri = url + str(i)
    print uri
    r = requests.get(uri)
    print r.text.decode('utf-8')
    time.sleep(0.1)