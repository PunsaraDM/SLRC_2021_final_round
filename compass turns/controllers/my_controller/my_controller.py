import math
from controller import Robot

robot = Robot()
TIME_STEP = 64
timestep = int(robot.getBasicTimeStep())
################Initializing components
ds = []
dsNames = ['ds_left', 'ds_right', 'ds_mid']
for i in range(3):
    ds.append(robot.getDevice(dsNames[i]))
    ds[i].enable(TIME_STEP)
    
compass = robot.getDevice("compass")
compass.enable(TIME_STEP)

wheels = []
wheelsNames = ['wheel1', 'wheel2', 'wheel3', 'wheel4']
for i in range(4):
    wheels.append(robot.getDevice(wheelsNames[i]))
    wheels[i].setPosition(float('inf'))
    wheels[i].setVelocity(0.0)
#########################################


############# Function to get compass which is used to get the orientation of robot
def getComDir():
    cVals = compass.getValues()
    xrVal = round(cVals[0],3)
    zrVal = round(cVals[2],3)
    
    rad = math.atan2(xrVal,zrVal)
    bearing = (rad - 1.5708)/math.pi * 180
    if(bearing < 0.0):
        bearing += 360
        
    
    bearing = bearing % 360
    bearing = round(bearing,3)
    comDirc = round(bearing / 90,3)

    return comDirc % 4,rad

########### Function to turn robot to a certain angle
def turnAng(a):

    cDirec = getComDir()[0]
    start_angle = getComDir()[0]
    a2 = round((start_angle + a/90)%4,3)
    a2 = a2 % 4

    q_prev = 1
    if(a < 0):
        q_prev = -1
    s = 1.0
    cw = 0
    while(getComDir()[0] != a2):
        cDirec = getComDir()[0]

        if(a2 < cDirec):
            cw = a2 + 4 - cDirec
        else:
            cw = a2 - cDirec
        
        if(cw <= 2):
            q = 1
        else:
            q = -1

        if(q_prev != q):
            s *= 0.5
            q_prev = q
       
        leftSpeed = 3.0*q*s
        rightSpeed = -3.0*q*s
        robot.step(1)
        wheels[0].setVelocity(leftSpeed)
        wheels[1].setVelocity(rightSpeed)
        wheels[2].setVelocity(leftSpeed)
        wheels[3].setVelocity(rightSpeed)

############Function to move robot forward
def go_forward():
    for i in range(4):
        wheels[i].setVelocity(2)

############Function to turn robot 90 degrees left       
def turn_left():
    turnAng(-90)

############Function to turn robot 90 degrees right    
def turn_right():
    turnAng(90)

############Function to turn robot 90 degrees right    
def turn_back():
    turnAng(90)
    turnAng(90)

############Function to stop robot
def stop():
    for i in range(4):
        wheels[i].setVelocity(0)
        
while robot.step(timestep) != -1:
    left_distance_sensor = ds[0].getValue()#####Get value of left distance sensor value
    right_distance_sensor = ds[1].getValue()#####Get value of right distance sensor value
    front_distance_sensor = ds[2].getValue()#####Get value of front distance sensor value
    
    ####Create if condition to check if front distance sensor value is less than 1500 from this we can recognize if a wall is infront
    ## Then inside the if condition above create a nested if condition to check if left distanace sensor value is less than 4000 and right distance sensor value is greater than 4000 from this we can recognize that there is no wall on the right side of the robot and make the robot turn right
    #Add the function turn_right() in the above if condtion
    ## Then create a elif condition to check if left distanace sensor value is greater than 4000 and right distance sensor value is less than 4000 from this we can recognize that there is no wall on the left side of the robot and make the robot turn left
    #Add the function turn_left() in the above elif condtion
    ####Now create a else statement to situations where there is no wall in front
    ## In that Statment if left distanace sensor value is greater than 4000 and right distance sensor value is greater than 4000 from this we can recognize that there is no wall on either side of the robot recognizing its the end 
    #Add the function stop() in the above if condtion
    ## Add a else condition to for the situation where robot is between wall and has the only option of going forward
    #Add the function go_forward() in the above else condtion
    
    turn_right()
    if front_distance_sensor < 1500:
        if left_distance_sensor < 4000 and right_distance_sensor > 4000:
            turn_right()
        elif left_distance_sensor > 4000 and right_distance_sensor < 4000:
            turn_left()
    else:
        if left_distance_sensor > 4000 and right_distance_sensor > 4000:
            stop()   
        else:
            go_forward()   
    pass
    


