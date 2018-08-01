%%%% StationaryRx %%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Setup for the first run
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% rx = Setup();

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Data Collection
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

starttime = datetime('now') + seconds(.5);
curtime = datetime;
while (string(curtime) ~= string(starttime))
    curtime = datetime;
%     disp(string(curtime));
end

%%%%%%%%%%%%%%%%%%
% Change These
%%%%%%%%%%%%%%%%%%
room = "testing";
NumSamples = 10;
DelayTime = .5;
%%%%%%%%%%%%%%%%%%

TotalRunTime = (NumSamples * DelayTime) / 60;
disp('This test will last for ~' + string(TotalRunTime) + ' minutes');
disp('or ~' + string(NumSamples * DelayTime) + ' seconds');

for runs = 1:NumSamples
    tic;
    for corrections = 1:8
        temp = rx();
    end
    eval(sprintf('%s_%d = temp;',room,runs));
    eval(sprintf('save("StationaryData/%s_%d.mat","%s_%d");', ...
        room,runs,room,runs));
    pause(DelayTime);
    toc;
end

save('workspace.mat');

%%%%%%%%%%%%%%%%%%%
% Choose Graph all
% or Graph range
%%%%%%%%%%%%%%%%%%%

start = 3;
finish = 6;

% GraphAll(room,NumSamples);
GraphRange(room,start,finish);

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
