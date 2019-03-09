close all;
addpath('../Functions');
addpath('../Data');
load('V2VnumCarriersPerLocation.mat');

%expecting a column vector (0-32)
fakeData1 = ceil(32*rand([4000,1]));
fakeData2 = ceil(32*rand([4000,1]));
fakeData3 = ceil(32*rand([4000,1]));
fakeData4 = ceil(32*rand([4000,1]));
fakeData5 = ceil(32*rand([4000,1]));

%move incomming data into variables
snrIndex = 50;
takeEveryXSamples = 1;
carriersPerLocationDataRun1 = cap_foxtrot(1:takeEveryXSamples:end,snrIndex);
carriersPerLocationDataRun2 = cap_golf(1:takeEveryXSamples:end,snrIndex);
carriersPerLocationDataRun3 = cap_hotel(1:takeEveryXSamples:end,snrIndex);
carriersPerLocationDataRun4 = cap_india(1:takeEveryXSamples:end,snrIndex);
carriersPerLocationDataRun5 = cap_juliet(1:takeEveryXSamples:end,snrIndex);

%extract the RM code curves
r = 2;
m = 5;
kBits = 0;
    for i = 0 : r
        kBits = kBits + nchoosek(m,i);
    end
RMWeights = RMWeightHier(r,m,false);

numOfLocations = length(carriersPerLocationDataRun1);

%initialize variables
revealedBitsRun1 = zeros(1,numOfLocations);
revealedBitsRun2 = zeros(1,numOfLocations);
revealedBitsRun3 = zeros(1,numOfLocations);
revealedBitsRun4 = zeros(1,numOfLocations);
revealedBitsRun5 = zeros(1,numOfLocations);

%look up bits of equivocation
for i = 1:numOfLocations
    revealedBitsRun1(1,i) = RMWeights(carriersPerLocationDataRun1(i,1)+1);
    revealedBitsRun2(1,i) = RMWeights(carriersPerLocationDataRun2(i,1)+1);
    revealedBitsRun3(1,i) = RMWeights(carriersPerLocationDataRun3(i,1)+1);
    revealedBitsRun4(1,i) = RMWeights(carriersPerLocationDataRun4(i,1)+1);
    revealedBitsRun5(1,i) = RMWeights(carriersPerLocationDataRun5(i,1)+1);
end

%create array for moving average
movingAve = [];
averageOver = 20;
for i = 1:averageOver
    movingAve(i) = 1/averageOver;
end

%calculate moving average and give bobs worst case and Eaves best
revealedBitsAveRun1 = ceil(conv(revealedBitsRun1,movingAve));
revealedBitsAveRun2 = floor(conv(revealedBitsRun2,movingAve));
revealedBitsAveRun3 = floor(conv(revealedBitsRun3,movingAve));
revealedBitsAveRun4 = floor(conv(revealedBitsRun4,movingAve));
revealedBitsAveRun5 = floor(conv(revealedBitsRun5,movingAve));

%plot data
figure(20);
hold on;
plot(revealedBitsAveRun1);
plot(revealedBitsAveRun2);
plot(revealedBitsAveRun3);
plot(revealedBitsAveRun4);
plot(revealedBitsAveRun5);
legend('Bob','Eave1','Eave2','Eave3','Eave4');
hold off;
grid on;
xlabel('Location');
ylabel('Equivocation in bits');
title(sprintf('V2V Coded With RM(%d,5) and \nMoving Average Over %d Locations at %.2f dB',...
    r,averageOver, 10*log10(snr(snrIndex))));
ylim([0 kBits+1]);
