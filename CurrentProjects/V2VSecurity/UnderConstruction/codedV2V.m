close all;
addpath('../Functions');

%expecting a column vector (0-32)
fakeData1 = ceil(32*rand([4000,1]));
fakeData2 = ceil(32*rand([4000,1]));
fakeData3 = ceil(32*rand([4000,1]));
fakeData4 = ceil(32*rand([4000,1]));
fakeData5 = ceil(32*rand([4000,1]));

carriersPerLocationDataRun1 = fakeData1;
carriersPerLocationDataRun2 = fakeData2;
carriersPerLocationDataRun3 = fakeData3;
carriersPerLocationDataRun4 = fakeData4;
carriersPerLocationDataRun5 = fakeData5;

r = 2;
m = 5;
RMWeights = RMWeightHier(r,m,false);

numOfLocations = length(carriersPerLocationDataRun1);

revealedBitsRun1 = zeros(1,numOfLocations);
revealedBitsRun2 = zeros(1,numOfLocations);
revealedBitsRun3 = zeros(1,numOfLocations);
revealedBitsRun4 = zeros(1,numOfLocations);
revealedBitsRun5 = zeros(1,numOfLocations);

for i = 1:numOfLocations
    revealedBitsRun1(1,i) = RMWeights(carriersPerLocationDataRun1(i));
    revealedBitsRun2(1,i) = RMWeights(carriersPerLocationDataRun2(i));
    revealedBitsRun3(1,i) = RMWeights(carriersPerLocationDataRun3(i));
    revealedBitsRun4(1,i) = RMWeights(carriersPerLocationDataRun4(i));
    revealedBitsRun5(1,i) = RMWeights(carriersPerLocationDataRun5(i));
end

figure(1);
hold on;
plot(revealedBitsRun1);
plot(revealedBitsRun2);
plot(revealedBitsRun3);
plot(revealedBitsRun4);
plot(revealedBitsRun5);

legend('Run1','Run2','Run3','Run4','Run5');
hold off;
xlabel('Location');
ylabel('Bits Revealed');
title('V2V Coded RM(2,5)');
ylim([0 17]);
