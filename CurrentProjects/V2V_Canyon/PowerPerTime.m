%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%% Power Per Time %%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% IN TESTING - Sept 26 2019   -> Try the other fiels

%%%%% Overview %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calculates the (average ?) power per a specified time unit
%   1)
% 
% 
%
% Files Used:
%A
%   - CarrierIndices.mat   ----> How?  Note that the indices are one off of
%   the latinCom indices for some reason -----------------
%   - Data files downampled to 20 Msamples per second (result of resampleData.m) 
%
% Files Produced:
%
%   - powerPerMilliSecond.mat 
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


clear;
disp(string(datetime));

%%%%% User Inputs %%%%%

filename = '/media/V2VData01/DiamondForkB_20Msps.mat'; % 
nfft = 2048;
sampleRate = 20;    % in MSamples/sec (20)
numSeconds = 0.001; % How many seconds we average over (0.001)

%%%%%%%%%%%%%%%%%%%%%%%

load('CanyonCarrierIndices.mat')
                                            b = b + 2;
samplesPerMegasample = 10^6;

% Calculate nfft's per numSeconds
samplesPerTime = sampleRate * samplesPerMegasample * numSeconds; % (20000)

mfr = dsp.MatFileReader(filename,'VariableName','y','SamplesPerFrame',samplesPerTime);  % Pull in (20000) samples at a time
dataCount = 0;              % Keeps track of samples accumulated
saveIndex = 1;
dataSum = 0;
readIndex = 1;
periodFinished = false;

displayCounter = 0;     % For sole purpose of notifying user about progress
counterCounter = 0;     % For sole purpose of notifying user about progress

largeData = mfr();
largeData = largeData.real + j*largeData.imag;

while ~isDone(mfr)
    % (Ignore the very last bit of data...)
    if ~isDone(mfr)       
        
        % When we finish up largeData (20000), pull in another chunk
        if ((readIndex + nfft) > samplesPerTime)
            overlap  = readIndex + nfft - samplesPerTime - 1;    % Amount extra that needs to be pulled
            amountRemaining = nfft - overlap;                    % Amount remaining from
            temp = largeData(readIndex:(readIndex + amountRemaining - 1)); 
            largeData = mfr();
            largeData = largeData.real + j*largeData.imag;
            if (isDone(mfr))
                break;
            end
            smallData = cat(1,temp, largeData(1:overlap));       
            readIndex = 1;
            periodFinished = true;
        else
            % Otherwise, fill smallData with data from largeData
            smallData = largeData(readIndex:(readIndex + nfft - 1));
            readIndex = readIndex + nfft;
        end
        
        % FFT and magnitude squared
        smallData = fft(smallData);
        smallData = abs(smallData) .^ 2;
        
        % Sum up over carriers
        for carrierCount = 1:64
            dataSum = dataSum + smallData(b(carrierCount,1),1); % Only care about the signal carriers, not noise carriers
        end
        dataCount = dataCount + nfft;           
        
        % Once the appropriate time has been summed over...
        if (periodFinished)
            periodFinished = false;
            dataCount = 0;
            displayCounter = displayCounter + 1;
                        
            % Data is stored in an array
            powerArray(saveIndex) = dataSum;
            dataSum = 0;
            saveIndex = saveIndex + 1;
            
            % And the user is notified
            if (displayCounter == 1000)
                counterCounter = counterCounter + 1;
                disp('Finished 1 seconds of data, for a total of ' + string(counterCounter));
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
