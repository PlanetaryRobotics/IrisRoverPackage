# CubeRoverPackage
Code to be included in the Iris Lunar Rover software package.

NOTE: The name `CubeRoverPackage` is legacy and predates the split of CubeRover and Iris Lunar Rover. This codebase is entirely unaffiliated with `CUBEROVER™`, a trademark of Astrobotic Technology.


## Development
### Highlevel Late-Stage Branching Philosophy:
The following is the policy for branching as of mid-2022 in the lead up to mission as final FSW release candidates are prepared.

#### General Branching Structure:
```
                             master
                               |                                     [GSW feature branches]
                               |                                  |- [feature / bug fix branches for GSW that's independent
                         master-staging - - - - - - - - - - - - - |-  of upcoming FSW RCs or needs access to the latest
                               |                                  |-  *released* FSW (i.e. not unreleased beta FSW)]
                               |
                       release-candidate
                           |   |   |
                     [RC feature branches]
[feature / bug fix branches for an FSW release-candidate (RC)]
```

#### Branch Definitions:
- `master`: Complete Iris Lunar Rover Software Package.
- `master-staging`: Draft code for `master`. Only contains **released** FSW (FSW that's on the Flight Rover).
- `release-candidate`: Draft FSW + GSW used for preparing code for a **Flight Release** (uploading code to the Flight Rover).
    - Every time code is uploaded to the flight rover, that constitutes a new **Flight Release**, at which point `release-candidate` is PR'd into `master-staging`.
- `[RC feature branches]`: Contain FSW and possibly GSW changes required for a new **Flight Release**. Only get pushed to `release-candidate` when the features are ready for a Flight Release.
- `[GSW feature branches]`: GSW-only feature branches that are independent of FSW feature changes / need access to the latest **Flight Release**.
    - This is important because some GSW features compile FSW. For the sake of accurate development, it's imperative the FSW this has access to reflects the latest state of the **released** FSW.
