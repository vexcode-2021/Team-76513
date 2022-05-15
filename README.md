While this code is nothing special in terms of robotics innovation, being the first major robotics codebase I have led, I'm happy with it.

Here are some of the cool features:

1. Controller rumbles at the sg3 time (last 30sec early season, last 15 sec at world's): https://github.com/vex-76513/code/blob/f981a61aeec66610aced2e40a231d95beb4adbb2/src/main.cpp#L150-L185

2. Auto MOGO grab using Vision and Ultrasonic: https://github.com/vex-76513/code/blob/f981a61aeec66610aced2e40a231d95beb4adbb2/src/autons/vision.hpp#L5-L50
   (I would NOT reuse this code as is, it's a mess and could be architected more modularly)

3. Monitor when the PID motion is stuck on a wall or MOGO, and move on: https://github.com/vex-76513/code/blob/f981a61aeec66610aced2e40a231d95beb4adbb2/src/autons/skills.hpp#L88-L107 and https://github.com/vex-76513/code/blob/f981a61aeec66610aced2e40a231d95beb4adbb2/src/subsystems/drivetrain.hpp#L55-L63
   (Again, don't use this, it could have a much better API)

4. My basic neutral goal grab autonomous that FINALLY worked reliably at World's: https://github.com/vex-76513/code/blob/f981a61aeec66610aced2e40a231d95beb4adbb2/src/autonomous.hpp#L6-L28
   (moral of the story don't use sensors when you only need to go straight, build a better drivetrain)

HUGE THANKS to:

1. OkapiLib folks (for literally all the complex drive, PID, odometry this does)
2. PROS makers (for the framework)
3. EZ-Template (for the auton-selector and ideas about stuck monitoring)
4. PROS-Grafana-CLI (while I didn't use this too much here, I'm really excited about its potential for spin up)
5. Belton High School for funding this, and Mr Giustino for teaching us and enabling us to do this
6. Finally, Vex and RECF for making this even exist


---

To see code history: <https://github.com/vex-76513/code/commits/pros>

This requires a custom version of OkapiLib at ../OkapiLib from https://github.com/vex-76513/vex-76513-OkapiLib/commit/3c5879de0cd2fa5e4e163e399a040e8aa5de1798
And EZ-Template at ../EZ-Template from https://github.com/EZ-Robotics/EZ-Template/commit/aeaf1785d5a580da01bc50094d1646d7a82383dc

To init environment:
```
        pros c apply --install --download --force --force-apply kernel@3.5.4
```
