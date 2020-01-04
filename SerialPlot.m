%/////////////////////////////////////////////////////////////////////
%                Final Project
%             McMaster University
%            2DP4 Microcontrollers
%                Lab Section L08
%  Gousheeg Kasivenkatasubbu, kasiveng, 400061872
%            MATLAB Version R2016b
%/////////////////////////////////////////////////////////////////////

clc; %Clear command window
clear; %Clear variables from memory

delete(instrfind); %Delete all open ports in MATLAB
s=serial('COM2','BaudRate',38400,'Terminator','CR'); %Serial object configuration
%Initalize variables
timestamp=0;
x=0;
center=2055;
%Open serial port
fopen(s);
s.status 

while(true)
    timestamp=timestamp+1;
    theta=fgetl(s); %Receiving and storing serial data
    
    %The condition below checks for negative angles
    if(theta(2)=='n')
        theta=-1*str2num(theta(3:end));
    else 
        theta=str2num(theta(3:end));
    end
    
    theta %To view serial data
    
    x=[x, theta];
    plot(x);
    %ylim([-100 100]); %To keep the plot from the beginning of the measurement
    %Comment above line and uncomment line below to shift the x axis of the plot as time progresses
    axis([timestamp-10 timestamp+10 -100,100]);
    title('Angle(\theta) vs Time(s), Gousheeg Kasivenkatasubbu, 400061872'); 
    ylabel('Angle(\theta)');
    xlabel('Time (t)');
    drawnow;
    
end
fclose('all');
delete s;
clear s;