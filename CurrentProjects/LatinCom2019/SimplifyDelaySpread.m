clear;
mfr = dsp.MatFileReader('delaySpreadResult.mat','VariableName','x','SamplesPerFrame',1);

index = 1;
d(1,64) = zeros();
indexOfPeak(1, 242998) = zeros();
while ~isDone(mfr)
    a = mfr();
    a = a.real + j*a.imag;
    b = abs(a) .^ 2;
    c = b(1,1:64);
%     plot(c); grid on; drawnow;
    indexOfPeak(1,index) = find(max(c));
    if indexOfPeak(1,index) ~= 1
        c = b(1,indexOfPeak(1,index):indexOfPeak(1,index) + 63);
        disp('Fixed index for run ' + string(index));
    end
    d = d + c;
    index = index + 1;
    if mod(index,10000) == 0
        disp('100000 done at ' + string(datetime));
    end
end
release(mfr);

e = d / index;

x = e;
save avgDelaySpreadResults.mat x;

% plot(e);
