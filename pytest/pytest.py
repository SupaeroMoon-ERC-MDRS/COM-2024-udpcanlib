from udpcanpy import NetworkHandler

nh = NetworkHandler()
res = nh.parse("/home/davidgmolnar/Documents/COM-2024/COM-2024-DBC/comms.dbc")
if(res != 0):
    print(f"Parse {res}")

res = nh.init()
if(res != 0):
    print(f"Init {res}")

res = nh.start()
if(res != 0):
    print(f"Start {res}")

remote = nh.getRemoteControl()
remdata = remote.access()

remdata.left_trigger = 10
remdata.right_trigger = 10

remote.update(remdata)
remdata2 = remote.access()

i = 0
print(i)
