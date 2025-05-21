from udpcanpy import NetworkHandler, RemoteControl, RaspiState
from raspistatechecker import RaspiStateChecker
from time import sleep

nh = NetworkHandler()
res = nh.parse("../../COM-2024-DBC/comms.dbc")
if(res != 0):
    print(f"Parse {res}")

res = nh.init(0)
if(res != 0):
    print(f"Init {res}")

res = nh.start()
if(res != 0):
    print(f"Start {res}")

remote = nh.getRemoteControl()
raspihandle = nh.getRaspiState()

data = RemoteControl()
raspi = RaspiState()

rsc = RaspiStateChecker()

while True:
    sleep(0.001)
    res = remote.access(data)
    if res == 0:
        print(f"=================\n\
              LB: {data.l_bottom}\n\
              LT: {data.l_top}\n\
              LR: {data.l_right}\n\
              LL: {data.l_left}\n\
              RB: {data.r_bottom}\n\
              RT: {data.r_top}\n\
              RR: {data.r_right}\n\
              RL: {data.r_left}\n\
              LS: {data.l_shoulder}\n\
              RS: {data.r_shoulder}\n\
              LTrigger: {data.left_trigger}\n\
              RTrigger: {data.right_trigger}\n\
              ThumbLX: {data.thumb_left_x}\n\
              ThumbLY: {data.thumb_left_y}\n\
              ThumbRX: {data.thumb_right_x}\n\
              ThumbRY: {data.thumb_right_y}\n\
                ")

    print('asd')
    if rsc.poll(raspi):
        print('yo')
        raspihandle.update(raspi)
        print('wtf')
        nh.pushRaspiState()
        print('hi')

    if data.e_stop:
        break

res = nh.stop()
if(res != 0):
    print(f"Stop {res}")

print("Adios")
