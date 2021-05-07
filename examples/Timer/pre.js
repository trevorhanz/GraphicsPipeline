const queryString = window.location.search;
const urlParams = new URLSearchParams(queryString);

numTimers = urlParams.get('numTimers')
if(numTimers)
  Module.arguments = [numTimers];
