
import requests

url = 'http://weixin.zhaopin.com/UserCenter/StateQueryCarBon.aspx?cid='


for i in range(91383,91383):
    uri = url + str(i)
    print uri
    r = requests.get(uri)
    print i
    print r.text