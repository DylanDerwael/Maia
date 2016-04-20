# Maia

Parallelized computer vision application, detecting blood and medical bandages for a robotic tool in laparoscopic operations.

Using:
- C++
- Qt
- OpenCV 

The purpose of the application is to create a visual processing element for a robotic tool. This
robotic tool will be utilized to assist a surgeon during an operation. During this project we will use a
laparoscopic surgery as an example for the image processing. During the operation the robot will be
required to be aware when internal bleeding occurs within the abdominal cavity so that it can
provide a medical bandage to the surgeon at the location of the bleeding as soon as possible, to
apply compression to the wound. Later on the application will be expanded to also detect the
medical bandages.
The application will not be limited to these situations but with having an eye to the future. The
project could be expanded to detect the organs within the abdominal cavity and construct a virtual
scenario of the abdominal cavity. This virtual mapping will aid both the robot and the surgeon in
completing the tasks at hand more accurately.
To build this application to be capable of handling all of the visual processing we will engineer it
upon the concept of parallel programming. This will help divide the tasks between the different
hardware cores of the CPU. Letting the application use all of the hardware resources will increase
the efficiency, scalability and load-balance capabilities of the application and thus making it future
proof.
