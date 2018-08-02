%%%% StationaryRx %%%%

%%%%%%%%%%%%%%%%%%
% File Setup
%%%%%%%%%%%%%%%%%%
starttime = datetime('now') + seconds(.5);
room = "harrison";
NumSamples = 30;
DelayTime = .5;

collectdata = true;
graphall = true;
graphrange = false;

% if graphrange = true
% then set the range
% with these numbers
start = 1;
finish = 1;
%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Setup for the first run
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if exist('rx','var') == 0
    rx = Setup();
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Data Collection
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if collectdata == true
    
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
            temp = rx();
        end
        eval(sprintf('%s_%d = temp;',room,runs));
        eval(sprintf('save("StationaryData/%s_%d.mat","%s_%d");', ...
            room,runs,room,runs));
        pause(DelayTime);
    end
    
    save('workspace.mat');
    
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
load('workspace.mat');
close all;
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
for runs = 1:NumSamples
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
load('workspace.mat');
close all;
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
for runs = beginnum:endnum
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
