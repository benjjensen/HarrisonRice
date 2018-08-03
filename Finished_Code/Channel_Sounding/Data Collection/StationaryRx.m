%%%% StationaryRx %%%%

%%%%%%%%%%%%%%%%%%
% File Setup
%%%%%%%%%%%%%%%%%%
starttime = datetime('now') + seconds(.5);      %Probably will end up changing this :)
room = "hallway";
NumSamples = 23;
DelayTime = .5;
dbthreshold = 5;

collectdata = false;
graphall = false;
graphrange = false;
pwelchit = false;
getcarriers = false;

% for use with
% graph range
start = 1;
finish = 1;
%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Setup for the first run
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if collectdata == true
    
    if exist('rx','var') == 0
        rx = Setup();
    end
    
        %%%Shouldn't there be an 'end' here? or do you not need it?
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Data Collection
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
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

%%%%%%%%%%%%%
% Pwelch Data
%%%%%%%%%%%%%

if pwelchit == true
    PwelchEverything(room,NumSamples);
end

%%%%%%%%%%%%%
% Separate
%%%%%%%%%%%%%

if getcarriers == true
    SeparateCarriers(room,NumSamples,dbthreshold);
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
FF = 20*FF;                             %%% What is the significance of 20 anyways?
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

function PwelchEverything(room,NumSamples)      % Nice
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

                %%% Basically all im getting from this is you wanted to
                %%% make my new program redundent so you could try and get
                %%% more lines and catch up to me. #NotGonnaHappen
                
                
                %%% But it does look really nice

