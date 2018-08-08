%%%% StationaryRx %%%%

%%%%%%%%%%%%%%%%%%
% File Setup
%%%%%%%%%%%%%%%%%%
starttime = datetime('now') + seconds(.5);      %Probably will end up changing this :)
room = "camacho";
NumSamples = 4000;
DelayTime = .5;
dbthreshold = 5;

collectdata = false;
makeonearray = true;
graphall = false;
graphrange = false;
pwelchit = false;
getcarriers = false;
useonearray = true;

% for use with
% graph range
start = 3100;
finish = 3120;
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
            temp = rx();
        end
        eval(sprintf('%s_%d = temp;',room,runs));
        eval(sprintf('save("StationaryData/%s_%d.mat","%s_%d");', ...
            room,runs,room,runs));
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
for runs = 1:NumSamples
    eval(sprintf('load("StationaryData/%s_%d.mat");',room,runs));
    eval(sprintf('%s_array(:,runs) = %s_%d(:);',room,room,runs));
end
eval(sprintf('save("StationaryData/%s_array.mat","%s_array");',room,room));
end

function GraphAll(room,NumSamples,useonearray)
close all;
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
if useonearray == false
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
if useonearray == true
    eval(sprintf('load("StationaryData/%s_array.mat");',room));

    for runs = 1:NumSamples
    obj = eval(sprintf('%s_array(:,%d)',room,runs));
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
end

function GraphRange(room,beginnum,endnum,useonearray)
close all;
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
if useonearray == false
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
end
if useonearray == true
    eval(sprintf('load("StationaryData/%s_array.mat");',room));
    for runs = beginnum:endnum
        obj = eval(sprintf('%s_array(:,%d)',room,runs));
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
end

function PwelchEverything(room,NumSamples,useonearray)
Nfft = 128;
if useonearray == false
    for runs = 1:NumSamples
        eval(sprintf('load("StationaryData/%s_%d.mat");',room,runs));
        obj = eval(sprintf('%s_%d',room,runs));
        YY = pwelch(obj(:),boxcar(Nfft),0,Nfft,'twosided');
        eval(sprintf('%s_pwelch_%d = 10*log10(abs(fftshift(YY)));',room,runs));
        eval(sprintf('save("StationaryData/%s_pwelch_%d.mat","%s_pwelch_%d");',room,runs,room,runs));
    end
end
if useonearray == true
    eval(sprintf('load("StationaryData/%s_array.mat");',room));
    for runs = 1:NumSamples
        obj = eval(sprintf('%s_array(:,%d)',room,runs));
        YY = pwelch(obj(:),boxcar(Nfft),0,Nfft,'twosided');
        eval(sprintf('%s_array_pwelch(:,%d) = 10*log10(abs(fftshift(YY)));',room,runs));
    end
    eval(sprintf('save("StationaryData/%s_array_pwelch.mat","%s_array_pwelch");',room,room));
end
end 

function SeparateCarriers(room,NumSamples,db,useonearray)
if useonearray == false
    for runs = 1:NumSamples
        eval(sprintf('load("StationaryData/%s_pwelch_%d.mat");',room,runs));
        eval(sprintf('signal = %s_pwelch_%d(1:2:end,1);',room,runs));
        eval(sprintf('noise = %s_pwelch_%d(2:2:end,1);',room,runs));
        difference = abs(signal(:,1) - noise(:,1));
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
if useonearray == true
    eval(sprintf('load("StationaryData/%s_array_pwelch.mat");',room));
    for runs = 1:NumSamples
        eval(sprintf('signal = %s_array_pwelch(1:2:end,%d);',room,runs));
        eval(sprintf('noise = %s_array_pwelch(2:2:end,%d);',room,runs));
        difference = abs(signal(:,1) - noise(:,1));
        count = 0;
        for carriers = 1:64
            if difference(carriers,1) >= db
                count = count + 1;
            end
        end
        eval(sprintf('%s_array_carriers(:,%d) = count;',room,runs));
        clear signal;
        clear noise;
        clear difference;
    end
    eval(sprintf( ...
        'save("StationaryData/%s_array_carriers.mat","%s_array_carriers");' ...
        ,room,room));
end
end

