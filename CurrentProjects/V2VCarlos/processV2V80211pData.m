%%% processV2V80211pData

% This file converts the data captured by the receiver and data
% acquisition card to approximately to I/Q (complex-valued) data 
% sampled at 20 Msamples/s. 
%
% The Van data is real-valued bandpass data sampled at 100 Msamples/s.
%
% Change the variable inFilename to the name of the data set you wish to
% convert. Nathan has to do his thing first to make it a .mat file.
%
% Change ther variable outFilename to the name of the file where you wish
% to store the I/Q (complex-valued) data sampled at 10 Msamples/s
%
% You can then reuse most of your code to process the data stored in
% outFilename.


load myFilterData80211p.mat; % h g

inFilename = '/media/V2V/dry-run_2019-03-18__18-36-24.mat';
outFilename = 'mdrTest.mat';

firdownsample = dsp.FIRDecimator(10,g);

mfr = dsp.MatFileReader(inFilename,'VariableName','data','SamplesPerFrame',10000);
mfw = dsp.MatFileWriter(outFilename,'VariableName','y');

while ~isDone(mfr)
    temp = mfr();
    x = (temp - 8192)/8192;
    y = firdownsample(x);
    mfw(y);
end
release(mfr);
release(mfw);