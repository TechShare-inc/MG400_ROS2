# MG400 Bringup
ROS2 package for launch files.

## Launch service server
```bash
ros2 launch mg400_bringup mg400.launch.py
```

## Connect launch server with MG400_Mock

```bash
ros2 launch mg400_bringup simulator.launch.py
```

or


```bash
ros2 launch mg400_bringup mg400.launch.py ip_address:=127.0.0.1
```