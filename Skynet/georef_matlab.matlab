function [Target_Latitude, Target_Longitude, Target_Altitude] = Georeferencing_Driver_4(plane_latitude, plane_longitude, plane_altitude,...
                                                                                        plane_roll, plane_pitch, plane_heading,...
                                                                                        gimbal_roll, gimbal_pitch, pixel_x, pixel_y, zoom_factor)

%GEOREFERENCING_DRIVER_4.m 
%    Returns the gps lat, long & alt of the given pixel in the camera image
%Author: Tim Wheeler
%Georeferencing Script
%February 12, 2011 
%For UCSD AUVSI Georeferencing


%INPUT:
% plane_latitude: the latitude of the plane [deg]
% plane_longitude: the longitude of the plane [deg]
% plane_altitude: the altitude MSL of the plane [m]
% plane_roll:  the roll of the plane [rad]
% plane_pitch: the pitch of the plane [rad]
% plane_heading: the heading or yaw of the plane [rad]
% gimbal_roll: the roll of the gimbal [rad]
% gimbal_pitch: the pitch of the gimbal [rad]
% pixel_x: the x pixel of the image, [0 -> (x_max - 1)]
% pixel_y: the y pixel of the image, [0 -> (y_max - 1)]
% zoom_factor: the zoom multiplier


%% UPDATING KNOWN VARIABLE INPUTS (CHANGING)

%transfor lat & long to radians
plane_latitude = plane_latitude * pi / 180;
plane_longitude = plane_longitude * pi / 180;


%% FIXED KNOWN VARIABLE INPUT (NON-CHANGING)

%Camera Data (for SD camera)
%**********NEED C CORRECTION MATRIX*************
x_fov= 46 * pi / 180; %[radians] Field of View, X Direction (Horizontal)
y_fov= 34 * pi / 180; %[radians] Field of View, Y Direction (Veritical)

x_pixels = 720; %The Number of Pixels in the Horizontal/X Direction (Total)
y_pixels = 486; %The Number of Pixels in the Vertical/Y Direction (Total)

%Geographic Information

%Earth Information WGS-84
a=6378137; %SEMIMAJOR AXIS 
b=6356752.3142; %SEMIMINOR AXIS 

%% Part A: Camera Centered rot Plane

%initial plane coordinates
plane_N_vector = [1,0,0]';
plane_E_vector = [0,1,0]';
plane_D_vector = [0,0,1]';

Camera_Point_Vector = EulerAngles_Plane(plane_D_vector, plane_roll, plane_pitch, plane_heading);
Camera_Up_Vector = EulerAngles_Plane(plane_N_vector, plane_roll, plane_pitch, plane_heading);


%% Part B: rot Gimbal

Q_gimbal_roll = Quaternion(gimbal_roll, Camera_Up_Vector(1), Camera_Up_Vector(2), Camera_Up_Vector(3));  %set up quaternion and rotate about camera_up_vector, which points forward down the plane
Camera_Point_Vector = Quaternion_Transform(Camera_Point_Vector, Q_gimbal_roll);  %rotate through gimbal roll
Camera_Up_Vector = Quaternion_Transform(Camera_Up_Vector, Q_gimbal_roll);  %rotate through gimbal roll

axis = cross(Camera_Point_Vector, Camera_Up_Vector);  %set up axis to rotate about
Q_gimbal_pitch = Quaternion(gimbal_pitch, axis(1),axis(2), axis(3));  %build the gimbal pitch quaternion, rotation about camera up
Camera_Point_Vector = Quaternion_Transform(Camera_Point_Vector, Q_gimbal_pitch) %rotate through gimbal pitch
Camera_Up_Vector = Quaternion_Transform(Camera_Up_Vector, Q_gimbal_pitch)  %rotate through gimbal pitch

%% Part C: zoom transform

FOV = [x_fov; y_fov; 1]; %F.O.V. Matrix

Scale = [1/zoom_factor,             0, 0;
                     0, 1/zoom_factor, 0;
                     0,             0, 1]; %Scaling Matrix

FOV_zoom_accounted = Scale*FOV; %take zoom into account

%% Part D: pixel transform

c_p = Camera_Point_Vector;
c_u = Camera_Up_Vector;
c_s = cross(c_p, c_u);

max_w = tan(FOV_zoom_accounted(1)/2)*2;
max_h = tan(FOV_zoom_accounted(2)/2)*2;

w = (max_w* pixel_x)/(x_pixels - 1) - max_w/2
h = (max_h* pixel_y)/(y_pixels - 1) - max_h/2

c_f = c_p - (w * c_s) + (h * c_u)
Pixel_Point_Vector = c_f;


%% Part E: Converting Plane Position to ECEF Coordinate Frame

f=a/(a-b); %Earth flatness
e=sqrt((1/f)*(2-(1/f))); %Earth eccenricity
N=a/(sqrt(1-(e^2)*sin(plane_latitude)^2)); %Length of Normal to the ellipsoid, extending from the surface to the intersection with the z-axis in ECEF

Plane_XYZ = [0,0,0]';
Plane_XYZ(1) = (N+plane_altitude)*cos(plane_latitude)*cos(plane_longitude);  %X
Plane_XYZ(2) = (N+plane_altitude)*cos(plane_latitude)*sin(plane_longitude);  %Y
Plane_XYZ(3) = (N*(1-e^2)+plane_altitude)*sin(plane_latitude)  ;             %Z

%% Part F: Using a 2D-ground plane estimation, find the pointed-to location

Ground_XYZ = [0,0,0]';
Ground_XYZ(1) = (N)*cos(plane_latitude)*cos(plane_longitude);  %X
Ground_XYZ(2) = (N)*cos(plane_latitude)*sin(plane_longitude);  %Y
Ground_XYZ(3) = (N*(1-e^2))*sin(plane_latitude); %Z

Pixel_XYZ = NED_to_ECEF(Pixel_Point_Vector, plane_latitude, plane_longitude);  %get the vector the pixel is pointed at in ECEF

dist = dot(Ground_XYZ - Plane_XYZ, Plane_XYZ) / dot(Pixel_XYZ, Plane_XYZ); % how far the gimbal points to the ground

Target_XYZ = Plane_XYZ + Pixel_XYZ * dist;  %get the target point

Target_GEO = ECEF_to_GEO(Target_XYZ, f, e, a);  %get the target point in lat,long, alt (Geodetic)

Target_Latitude = Target_GEO(1) * 180 / pi;
Target_Longitude = Target_GEO(2) * 180 / pi - 180;
Target_Altitude = Target_GEO(3) - N;

end