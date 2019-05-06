%%% processV2VSingleCarrierData

% This file converts the data captured by the receiver and data
% acquisition card to approximately to I/Q (complex-valued) data 
% sampled at 11.2 ksamples/s. 
%
% The input data is real-valued bandpass data sampled at 11.2 Msamples/s.
%
% Change the variable inFilename to the name of the data set you wish to
% convert. Nathan has to do his thing first to make it a .mat file.
%
% Change ther variable outFilename to the name of the file where you wish
% to store the I/Q (complex-valued) data sampled at 11.2 ksamples/s
%
% The resampling is performed in two steps: downsample by 4, then
% downsample by 250.


load myFilterDataSingleCarrier.mat; % h1 g1 h2

inFilename = 'singleCarrierData/echo_2019-03-19__14-26-56.mat';
outFilename = 'singleCarrierData/echo_2019-03-19__14-26-56_downsample.mat';

firdownsample4 = dsp.FIRDecimator(4,g1);
firdownsample250 = dsp.FIRDecimator(250,h2);

mfr = dsp.MatFileReader(inFilename,'VariableName','data','SamplesPerFrame',1000000);
mfw = dsp.MatFileWriter(outFilename,'VariableName','y');

while ~isDone(mfr)
    temp = mfr();
    x = (temp - 8192)/8192;
    ytemp = firdownsample4(x);
    y = firdownsample250(ytemp);
    mfw(y);
end
release(mfr);
release(mfw);