%%%%% DELAY SPREAD %%%%%
clear; 
% close all;

mfr = dsp.MatFileReader('ds_PowerArray.mat','VariableName','ds_PowerArray','SamplesPerFrame',1);

% Downsample Ref Signal to 20 Ms, pwelch first chunk

Nfft = 2048;

load('RawOrResampledData/V2VReference_20Msps.mat');
ref = y(1:20000, end);                                  % Pull in 1 ms of data
refSignal = pwelch(ref,boxcar(Nfft),0,Nfft,'twosided'); % Pwelch it

% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Checking for frequency shifting
% numruns = 242998;
% split = 10;
% indicesPerSplit = numruns / split;
% indexNextSplit = indicesPerSplit;
% splitNum = 1;
% l = 1;
% for i = 1:16
%     a(i,:) = i:16:2048;
% end
% maxIndices(242998,1) = zeros();
% while ~isDone(mfr)
%     data = mfr();
%     max = 0;
%     maxindex = 0;
%     for i = 1:16
%         avg = mean(data(1,a(i,:)));
%         if avg > max
%             max = avg;
%             maxindex = i;
%         end
%     end
%     maxIndices(l,1) = maxindex;
%     l = l + 1;
%     if l >= indexNextSplit
%         disp(string(splitNum) + ' / ' + string(split) + ' at ' + string(datetime));
%         splitNum = splitNum + 1;
%         indexNextSplit = indexNextSplit + indicesPerSplit;
%     end
% end
% k = 1;
% for i = 1:242998
%     if maxIndices(i,:) ~= maxIndices(1,:)
%         notequal(k) = i;
%         k = k+1;
%     end
% end
% [~, runs] = size(notequal);
% longestRun = 1;
% currentRun = 1;
% indexOfRun = 1;
% for i = 1:runs-1
%     if notequal(i) + 1 == notequal(i+1)
%         currentRun = currentRun + 1;
%     else
%         currentRun = 1;
%     end
%     if currentRun > longestRun
%         longestRun = currentRun;
%         indexOfRun = i;
%     end
% end
%     
% disp('Done at ' + string(datetime));
% 
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Looping through the file, set up an array of zeros, then copy over
% ONLY the carriers from the ms data ./ carriers from reference signal

index = 1;
data = mfr();
carrierIndices = find(data > 0.5); % May need to adjust
[~, I] = sort(data, 'descend');
carrierInd = sort(I(1:64));
refCarriers = find(refSignal > 0.005);
refDivider = refSignal(refCarriers,1)';

numrows = 242998;
divisor = 10;
divisions = numrows / divisor;
indexDivisions = divisions;
divisionNum = 1;

% H = zeros(numrows,Nfft);
H = zeros(1,Nfft);
% h = zeros(numrows,Nfft);
h = zeros(1,Nfft);
mfw = dsp.MatFileWriter('delaySpreadResult_2.mat');
while ~isDone(mfr)    
%     H(index,carrierIndices) = data(1,carrierIndices) ./ refDivider; % Only copy over the carriers, after dividing out reference
    H(1,carrierIndices) = data(1,carrierIndices) ./ refDivider;
%     h(index,:) = ifft(H(index,:)); % IFFT the result
%     h(index,:) = ifft(H);
    h = ifft(H);
%     m = abs(h) .^ c2;
%     plot(m); grid on;
    mfw(h);
    index = index + 1;
    data = mfr();
    
    if index >= indexDivisions
        disp(string(divisionNum) + ' / ' + string(divisor) + ' done at ' + string(datetime));
        divisionNum = divisionNum + 1;
        indexDivisions = indexDivisions + divisions;
    end
end

% H(index,:) = zeros(1, 2048);  % Initialize it all to zero
% H(index,carrierIndices) = data(1,carrierIndices) ./ refSignal(refCarriers,1)'; % Only copy over the carriers, after dividing out reference
% h(index,:) = ifft(H(index,:)); % IFFT the result
H(1,carrierIndices) = data(1,carrierIndices) ./ refDivider;
h = ifft(H);
mfw(h);
disp('Done at ' + string(datetime));

% Notes - Make sure to compare spikes between power and ref to ensure no
% frequency shifting occured.


