% PowerPerTime  -  TESTING!!!!!
clear;
disp(string(datetime));

%%%%% User Inputs %%%%%

% filename = '/media/V2V/Sundance-To-US189-25June2019_20Msps.mat'; % Down
filename = '/media/V2V/US189-To-Sundance-25June2019_20Msps.mat'; % Up
nfft = 2048;
sampleRate = 20;    % in MSamples/sec (10)
numSeconds = 0.001; % How many seconds we average over (0.001)

%%%%%%%%%%%%%%%%%%%%%%%

load('carrierIndices.mat')

samplesPerMegasample = 10^6;

% Calculate nfft's per numSeconds
samplesPerTime = sampleRate * samplesPerMegasample * numSeconds; % (10000)

mfr = dsp.MatFileReader(filename,'VariableName','y','SamplesPerFrame',nfft);
count = 0;      % Keeps track of samples accumulated
index = 1;
total = 0;

displayCounter = 0;     % For sole purpose of notifying user about progress
counterCounter = 0;     % For sole purpose of notifying user about progress


while ~isDone(mfr)
    data = mfr();
    data = data.real + j*data.imag;     %%%%%%%
    if ~isDone(mfr)
        %         data = data .* blackman(nfft);                  %%%% Should I be doing this?
        data = fft(data);
        data = abs(data) .^ 2;
        
        % Sum up over carriers;
        for carrierCount = 1:64
            total = total + data(a(carrierCount,1),1);
        end
        count = count + nfft;
        
        % Once the appropriate time has been summed over...
        if (count >= samplesPerTime)                         % Note that this will grab a little bit extra 
            count = 0;
            displayCounter = displayCounter + 1;
            
            
            % Data is stored in an array          
            powerArray(index) = total;
            total = 0;
            index = index + 1;
            
            % And the user is notified
            if (displayCounter == 1000*30)
                counterCounter = counterCounter + 1;
                disp('Finished 30 seconds of data, for a total of ' + string(counterCounter));
                displayCounter = 0;
            end
        end
    end
   
end

release(mfr);

disp('Finished - ' + string(datetime));


%%

figure()
plot(powerArray)
