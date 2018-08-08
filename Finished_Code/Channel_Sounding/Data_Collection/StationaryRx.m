%%%% StationaryRx %%%%

%%%%%%%%%%%%%%%%%%
% File Setup
%%%%%%%%%%%%%%%%%%
starttime = datetime('now') + seconds(.5);      %Probably will end up changing this :)
room = "camacho";
NumSamples = 4000;
DelayTime = .5;
dbthreshold = 24;       %threshold for getcarriers

collectdata = false;    %used for data collection! (pretty self explanatory)
makeonearray = false;   %condenses the data files into one array
graphall = false;       %graphs all of the data
graphrange = false;     %graphs a set range from the data
pwelchit = false;       %pwelches the data array
getcarriers = false;    %generates carriers array at set db level
compare = true;         %creates a single histogram using carriers array
useonearray = true;     %set to true if the array from makeonearray is what you want to use

datafolder= "StationaryData/";

% for use with
% graph range
start = 3100;           %for use with graphrange
finish = 3120;          %for use with graphrange
%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Setup for the first run
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if collectdata == true
    
    if exist('rx','var') == 0
        rx = Setup();
    end
        
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Data Collection
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    curtime = datetime;
    while (string(curtime) ~= string(starttime))
        curtime = datetime;
        %     disp(string(curtime));
    end
    
    TotalRunTime = (NumSamples * (DelayTime + .07)) / 60;
    disp('This test will last for ~ ' + string(TotalRunTime) + ' minutes');
    disp('or for ~ ' + string(NumSamples * DelayTime) + ' seconds');
    
    for runs = 1:NumSamples
        for corrections = 1:8
            measurement = rx();
        end
        
        filename = datafolder + room + "_" + runs + ".mat";
        save(filename, "measurement");
        
        pause(DelayTime);
    end
        
end

%%%%%%%%%%%%%
% Join Arrays
%%%%%%%%%%%%%

if makeonearray == true
    JoinArrays(room,NumSamples);
end

%%%%%%%%%%%%%
% Graph All
%%%%%%%%%%%%%

if graphall == true
    GraphAll(room,NumSamples,useonearray);
end

%%%%%%%%%%%%%
% Graph Range
%%%%%%%%%%%%%

if graphrange == true
    GraphRange(room,start,finish,useonearray);
end

%%%%%%%%%%%%%
% Pwelch Data
%%%%%%%%%%%%%

if pwelchit == true
    PwelchEverything(room,NumSamples,useonearray);
end

%%%%%%%%%%%%%
% Separate
%%%%%%%%%%%%%

if getcarriers == true
    SeparateCarriers(room,NumSamples,dbthreshold,useonearray);
end

%%%%%%%%%%%%%
% Compare
%%%%%%%%%%%%%

if compare == true
    CompareCarriers(room,NumSamples,useonearray);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Functions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [rx] = Setup()
plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;
end

function JoinArrays(room,NumSamples)
% preallocating:
measurements = zeroes(128, NumSamples);
for runs = 1:NumSamples
    infilename = datafolder + room + "_" + runs + ".mat";
    measurement = [];
    load(infilename, "measurement");
    measurements(:, runs) = measurement(:);
end
outfilename = datafolder + room + "_array.mat";
save(outfilename, "measurements");
end

function GraphAll(room,NumSamples,useonearray)
close all;
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
if useonearray == false
    for runs = 1:NumSamples
        infilename = datafolder + room + "_" + runs + ".mat";
        measurement = [];
        load(infilename, "measurement");
        
        YY = pwelch(measurement(:),boxcar(Nfft),0,Nfft,'twosided');
        YYplot = 10*log10(abs(fftshift(YY)));
        figure();
        stem(FF,YYplot);
        grid on;
        title(sprintf('%s_%d',room,runs), 'Interpreter','none');
        xlabel('frequency (MHz)');
        ylabel('Magnitude (dB)');
        ax = gca;
        ax.Children.BaseValue = -80;
        hold on;
        plot(FF(1:2:end),YYplot(1:2:end),'o','MarkerFaceColor','k');
        hold off;
    end
end
if useonearray == true
    infilename = datafolder + room + "_array.mat";
    measurements = [];
    load(infilename, "measurements");

    for runs = 1:NumSamples
        measurement = measurements(:,runs);
        YY = pwelch(measurement(:),boxcar(Nfft),0,Nfft,'twosided');
        YYplot = 10*log10(abs(fftshift(YY)));
        figure();
        stem(FF,YYplot);
        grid on;
        title(sprintf('%s_%d',room,runs), 'Interpreter','none');
        xlabel('frequency (MHz)');
        ylabel('Magnitude (dB)');
        ax = gca;
        ax.Children.BaseValue = -80;
        hold on;
        plot(FF(1:2:end),YYplot(1:2:end),'o','MarkerFaceColor','k');
        hold off;
    end
end
end

function GraphRange(room,beginnum,endnum,useonearray)
close all;
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
if useonearray == false
    for runs = beginnum:endnum
        infilename = datafolder + room + "_" + runs + ".mat";
        measurement = [];
        load(infilename, "measurement");
        
        YY = pwelch(measurement(:),boxcar(Nfft),0,Nfft,'twosided');
        YYplot = 10*log10(abs(fftshift(YY)));
        figure();
        stem(FF,YYplot);
        grid on;
        title(sprintf('%s_%d',room,runs), 'Interpreter','none');
        xlabel('frequency (MHz)');
        ylabel('Magnitude (dB)');
        ax = gca;
        ax.Children.BaseValue = -80;
        hold on;
        plot(FF(1:2:end),YYplot(1:2:end),'o','MarkerFaceColor','k');
        hold off;
    end
end
if useonearray == true
    infilename = datafolder + room + "_array.mat";
    measurements = [];
    load(infilename, "measurements");
    
    for runs = beginnum:endnum
        measurement = measurements(:,runs);
        YY = pwelch(measurement(:),boxcar(Nfft),0,Nfft,'twosided');
        YYplot = 10*log10(abs(fftshift(YY)));
        figure();
        stem(FF,YYplot);
        grid on;
        title(sprintf('%s_%d',room,runs), 'Interpreter','none');
        xlabel('frequency (MHz)');
        ylabel('Magnitude (dB)');
        ax = gca;
        ax.Children.BaseValue = -80;
        hold on;
        plot(FF(1:2:end),YYplot(1:2:end),'o','MarkerFaceColor','k');
        hold off;
    end
end
end

function PwelchEverything(room,NumSamples,useonearray)
Nfft = 128;
if useonearray == false
    for runs = 1:NumSamples
        infilename = datafolder + room + "_" + runs + ".mat";
        measurement = [];
        load(infilename, "measurement");
        
        YY = pwelch(measurement(:),boxcar(Nfft),0,Nfft,'twosided');
        pwelched_measurement = 10*log10(abs(fftshift(YY)));
        
        outfilename = datafolder + "pwelch_" + room + "_" + runs + ".mat";
        save(outfilename, "pwelched_measurement");
    end
end
if useonearray == true
    infilename = datafolder + room + "_array.mat";
    measurements = [];
    load(infilename, "measurements");
    
    pwelched_measurements = zeroes(128, NumSamples);
    for runs = 1:NumSamples
        measurement = measurements(:,runs);
        YY = pwelch(measurement(:),boxcar(Nfft),0,Nfft,'twosided');
        pwelched_measurements(:,runs) = 10*log10(abs(fftshift(YY)));
    end
    outfilename = datafolder + "pwelch_" + room + "_array.mat";
    save(outfilename, "pwelched_measurements");
end
end 

function SeparateCarriers(room,NumSamples,db,useonearray)
if useonearray == false
    for runs = 1:NumSamples
        infilename = datafolder + "pwelch_" + room + "_" + runs + ".mat";
        pwelched_measurement = [];
        load(infilename, "pwelched_measurement");
        
        signal = pwelched_measurement(1:2:end,1);
        noise = pwelched_measurement(2:2:end,1);
        
        difference = abs(signal(:,1) - noise(:,1));
        count = 0;
        for carriers_array = 1:64
            if difference(carriers_array,1) >= db
                count = count + 1;
            end
        end
        carriers = count;
        
        outfilename = datafolder + "carriers_" + room + "_" + runs + ".mat";
        save(outfilename, "carriers");
    end
end
if useonearray == true
    infilename = datafolder + "pwelch_" + room + "_array.mat";
    pwelched_measurements = [];
    load(infilename, "pwelched_measurements");
    
    carriers_array = zeroes(1, NumSamples);
    for runs = 1:NumSamples
        signal = pwelched_measurements(1:2:end, runs);
        noise = pwelched_measurements(2:2:end, runs);
        
        difference = abs(signal(:,1) - noise(:,1));
        count = 0;
        for carriers_array = 1:64
            if difference(carriers_array,1) >= db
                count = count + 1;
            end
        end
        carriers_array(:, runs) = count;
        clear signal;
        clear noise;
        clear difference;
    end
    outfilename = datafolder + "carriers_" + room + ".mat";
    save(outfilename, "carriers_array");
end
end

function CompareCarriers(room,NumSamples,useonearray) % needs testing
if useonearray == false
    room_history = zeroes(NumSamples);
    for runs = 1:NumSamples
        infilename = datafolder + "carriers_" + room + "_" + runs + ".mat";
        carriers = [];
        load(infilename, "carriers");
        
        room_history(runs) = carriers;
    end
    histogram(room_history);
end
if useonearray == true
    infilename = datafolder + "carriers_" + room + ".mat";
    carriers_array = [];
    load(infilename, "carriers_array");
    
    histogram(carriers_array);
end
end

