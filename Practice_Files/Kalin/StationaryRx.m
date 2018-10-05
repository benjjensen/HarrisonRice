%%%% StationaryRx %%%%

%%%%%%%%%%%%%%%%%%
% File Setup
%%%%%%%%%%%%%%%%%%
starttime = '07-Aug-2018 19:07:00';
room = "camacho";
NumSamples = 4000;
DelayTime = .5;
dbthreshold = 5;

collectdata = false;
graphall = false;
graphrange = true;
    start = 750;
    finish = 780;
pwelchit = false;
getcarriers = false;
compare = false;

%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Setup for the first run
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if collectdata == true
    
    if exist('rx','var') == 0
        rx = Setup();
    end
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Data Collection
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    curtime = datetime;
    while (string(curtime) ~= string(starttime))
        curtime = datetime;
            disp(string(curtime));
    end
    
    TotalRunTime = (NumSamples * (DelayTime + .07)) / 60;
    disp('This test will last for ~ ' + string(TotalRunTime) + ' minutes');
    disp('or for ~ ' + string(NumSamples * DelayTime) + ' seconds');
    
    for runs = 1:NumSamples
        for corrections = 1:8
            temp = rx();
        end
        eval(sprintf('%s_%d = temp;',room,runs));
        eval(sprintf('save("StationaryData/%s_%d.mat","%s_%d");', ...
            room,runs,room,runs));
        pause(DelayTime);
    end
        
end

%%%%%%%%%%%%%
% Graph All
%%%%%%%%%%%%%

if graphall == true
    GraphAll(room,NumSamples);
end

%%%%%%%%%%%%%
% Graph Range
%%%%%%%%%%%%%

if graphrange == true
    GraphRange(room,start,finish);
end

%%%%%%%%%%%%%
% Pwelch Data
%%%%%%%%%%%%%

if pwelchit == true
    PwelchEverything(room,NumSamples);
end

%%%%%%%%%%%%%
% Carriers
%%%%%%%%%%%%%

if getcarriers == true
    SeparateCarriers(room,NumSamples,dbthreshold);
end

%%%%%%%%%%%%%
% Compare
%%%%%%%%%%%%%

if compare == true
    CompareCarriers(room,NumSamples);
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

function GraphAll(room,NumSamples)
close all;
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
for runs = 1:NumSamples
    eval(sprintf('load("StationaryData/%s_%d.mat");',room,runs));
    obj = eval(sprintf('%s_%d',room,runs));
    YY = pwelch(obj(:),boxcar(Nfft),0,Nfft,'twosided');
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

function GraphRange(room,beginnum,endnum)
close all;
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
for runs = beginnum:endnum
    eval(sprintf('load("StationaryData/%s_%d.mat");',room,runs));
    obj = eval(sprintf('%s_%d',room,runs));
    YY = pwelch(obj(:),boxcar(Nfft),0,Nfft,'twosided');
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
clear runs;
end

function PwelchEverything(room,NumSamples)
Nfft = 128;
for runs = 1:NumSamples
    eval(sprintf('load("StationaryData/%s_%d.mat");',room,runs));
    obj = eval(sprintf('%s_%d',room,runs));
    YY = pwelch(obj(:),boxcar(Nfft),0,Nfft,'twosided');
    eval(sprintf('%s_pwelch_%d = 10*log10(abs(fftshift(YY)));',room,runs));
    eval(sprintf('save("StationaryData/%s_pwelch_%d.mat","%s_pwelch_%d");',room,runs,room,runs));
end
end

function SeparateCarriers(room,NumSamples,db)
for runs = 1:NumSamples
    eval(sprintf('load("StationaryData/%s_pwelch_%d.mat");',room,runs));
    eval(sprintf('signal = %s_pwelch_%d(1:2:end,1);',room,runs));
    eval(sprintf('noise = %s_pwelch_%d(2:2:end,1);',room,runs));
    difference = abs(signal(:,1) - noise(:,1))
    count = 0;
    for carriers = 1:64
        if difference(carriers,1) >= db
            count = count + 1;
        end
    end
    eval(sprintf('%s_carriers_%d = count;',room,runs));
    eval(sprintf( ...
        'save("StationaryData/%s_carriers_%d.mat","%s_carriers_%d");' ...
        ,room,runs,room,runs));
end
end

function CompareCarriers(room,NumSamples) % needs testing
for runs = 1:NumSamples
    eval(sprintf('load("StationaryData/%s_carriers_%d.mat");',room,runs));
    room_history(runs) = eval(sprintf('%s_carriers_%d;',room,runs));
end
histogram(room_history);
end
