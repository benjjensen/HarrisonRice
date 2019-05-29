% This script was generated to test the burst mode feature of the Pluto
% Radios to see if the buffer problem was fixed (previously, the first
% seven plots generated would be from a previous location). 

% To test, I first gathered ten frames of data without using burst mode,
% and the plots were incorrect, as expected. I then turned on burst mode by
% uncommenting the two lines below, and that time everything worked
% correctly

%%
clear;

%%% Typical Pluto Radio Setup stuff
plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;

%%% Uncomment to enable burst mode
% rx.EnableBurstMode = true;
% rx.NumFramesInBurst = 1;


x = zeros(2048,10);

% Endlessly gathers ten data samples, pwelches, and then plots
while(1)
    for i = 1:10
        x(:,i) = rx();
    end
    Nfft = 128;
    a = pwelch(x,boxcar(Nfft),0,Nfft,'twosided');
    
    for i = 1:10
        figure(i)
        plot(a(:,i));
    end
    
    pause(20);
    beep;
end