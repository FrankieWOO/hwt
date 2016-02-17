% MATLAB code plots data from two Lilypad Accelerometer sensors and two EMG sensors
% Reads data from an Arduino on USB serial port. 
% Lilypad Accelerometer has three outputs (x, y, z)
% EMG sensor has a single output (EMG analog voltage)

clear;clc;clearvars;

%Plot initial graphs
figure(1);
ax(1) = subplot(2,2,1);
plotGraph(1) = plot(0,0);
 
ax(2) = subplot(2,2,2);
plotGraph(2) = plot(0,0);
 
ax(3) = subplot(2,2,3);
plotGraph(3) = plot(0,0);
 
ax(4) = subplot(2,2,4);
plotGraph(4) = plot(0,0);

numberOfSensors = 1; % if using a multiplexor, if not, set to 1
numberOfExpectedInputs = 8; % per sensor (per serial line input)

% Initial starting conditions
Xa = 0;
Ya = 0;
Za = 0;
EMG1 = 0;
EMG2 =0;
xWidth = 10;      % width of sliding window
time = 0;
count = 0;

% Open port
disp('Opening connection ...')
s = serial('/dev/ttyUSB0') 
set(s, 'DataBits', 8);
set(s, 'StopBits', 1);
set(s, 'BaudRate', 9600);
set(s, 'Parity', 'none');
fopen(s);

% Check to see if all components have initalised
disp(fgets(s)); % display initalision
pause(1);
disp(fgets(s)); % Connected!

tic

try
    flag=1; 
    count=1;
  
  while true
       expectedSensor=1;  
            while expectedSensor <= 2
            raw_dat = fscanf(s);
            formatted=[];
            
            for i=1:numberOfExpectedInputs
                
                [TK raw_dat]=strtok(raw_dat,',');
                try
                    formatted(i)=str2num(TK);
                catch
                end
            end
            dat=formatted';
            
            % If something went wrong with this reading           
            if~(~isempty(dat) && isfloat(dat) && size(dat,1)==numberOfExpectedInputs) % as data is sometimes dropped
                disp('wrong serial size')
                sensor=expectedSensor;
                Xa1(count) = NaN;
                Ya1(count) = NaN;
                Za1(count) = NaN;
                Xa2(count) = NaN;
                Ya2(count) = NaN;
                Za2(count) = NaN;              
                EMG1(count) = NaN;
                EMG2(count) = NaN;
                
                          
                flag=true;       
            else
                % Read raw data from serial
                Xa1(count) = dat(1);
                Ya1(count) = dat(2);
                Za1(count) = dat(3);
                Xa2(count) = dat(4);
                Ya2(count) = dat(5);
                Za2(count) = dat(6);              
                EMG1(count) = dat(7);
                EMG2(count) = dat(8);
                   
                flag=true;
            end
                if flag==true
                    if expectedSensor==1
                        expectedSensor=2;
                    else
                        break;
                    end
                end
        end
        time(count) = toc; % assumes all readings (from all sensors), taken at exactly the same time
        
        % Speed up graphing
        time_subset=time(time > time(count)-xWidth);
        Xa_1=Xa1(time > time(count)-xWidth);
        Ya_1=Ya1(time > time(count)-xWidth);
        Za_1=Za1(time > time(count)-xWidth);
        Xa_2=Xa2(time > time(count)-xWidth);
        Ya_2=Ya2(time > time(count)-xWidth);
        Za_2=Za2(time > time(count)-xWidth);
        EMG1_sub = EMG1(time > time(count)-xWidth);
        EMG2_sub = EMG2(time > time(count)-xWidth);
        
         % Plot first Sensor with Z (red), X (blue), Y (green)
         plot(ax(1), time_subset,Za_1,'-r', time_subset, Xa_1, '-b', time_subset, Ya_1,'-g', 'linewidth',2);
         axis(ax(1),[time(count)-xWidth time(count) -2 2]);
         xlabel(ax(1), 'Time (s)');
         ylabel(ax(1), 'Acceleration Sensor 1 (g)');
         
         % Plot second Sensor with Z (red), X (blue), Y (green)
         plot(ax(2), time_subset,Za_2,'-r', time_subset, Xa_2, '-b', time_subset, Ya_2,'-g', 'linewidth',2);
         axis(ax(2),[time(count)-xWidth time(count) -2 2]);
         xlabel(ax(2), 'Time (s)');
         ylabel(ax(2), 'Acceleration Sensor 2 (g)');
         
         % Plot EMG analog signal from first component
         plot(ax(3), time_subset, EMG1_sub, '-g', 'linewidth', 3);
         axis(ax(3),[time(count)-xWidth time(count) -200 700]);
         xlabel(ax(3), 'Time (s)');
         ylabel(ax(3), 'EMG 1 ');
         
         % Plot EMG analog signal from second component
         plot(ax(4), time_subset, EMG2_sub, '-g', 'linewidth', 3);
         axis(ax(4),[time(count)-xWidth time(count) -200 700]);
         xlabel(ax(4), 'Time (s)');
         ylabel(ax(4), 'EMG 2 ');
        
         drawnow;
        count = count + 1;
    end
    
catch err
    fclose(s);
    disp('crashed, closed serial')
    rethrow(err);
    
end

fclose(s);

