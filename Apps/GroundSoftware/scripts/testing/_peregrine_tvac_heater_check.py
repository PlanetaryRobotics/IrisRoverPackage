import dateutil.parser
from scripts.testing._peregrine_tvac_fetcher import *

PLOT_OUT_FILE: Final[str] = './out/databases/plots/__HEATER_CHECK.png'

packets_raw = [
    (("2023-01-16T02:31:08.482Z"), "FF000078"),
    (("2023-01-16T02:31:34.591Z"), "FF000078"),
    (("2023-01-16T02:32:00.731Z"), "D50008180007000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:32:00.773Z"), "FF000078"),
    (("2023-01-16T02:32:26.797Z"), "FF000078"),
    (("2023-01-16T02:32:52.911Z"), "FF000078"),
    (("2023-01-16T02:33:19.059Z"), "D50008180008000000380000000000670022408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:33:19.101Z"), "FF000078"),
    (("2023-01-16T02:33:45.120Z"), "FF000078"),
    (("2023-01-16T02:34:11.225Z"), "FF000078"),
    (("2023-01-16T02:34:37.377Z"), "D50008180009000000380000000000660022408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:34:37.419Z"), "FF000078"),
    (("2023-01-16T02:35:03.430Z"), "FF000078"),
    (("2023-01-16T02:35:16.523Z"), "D5000818000A000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:35:29.525Z"), "FF000079"),
    (("2023-01-16T02:35:55.674Z"), "D5000818000B000000380000000000670042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:35:55.715Z"), "FF000079"),
    (("2023-01-16T02:36:21.740Z"), "FF000079"),
    (("2023-01-16T02:36:47.842Z"), "FF000079"),
    (("2023-01-16T02:37:13.986Z"), "D5000818000C000000380000000000670022408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:37:14.028Z"), "FF000079"),
    (("2023-01-16T02:37:40.052Z"), "FF000079"),
    (("2023-01-16T02:38:06.164Z"), "FF000079"),
    (("2023-01-16T02:38:32.331Z"), "D5000818000D000000380000000000660022408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:38:32.373Z"), "FF000079"),
    (("2023-01-16T02:38:58.409Z"), "FF000079"),
    (("2023-01-16T02:39:24.522Z"), "FF000079"),
    (("2023-01-16T02:39:50.664Z"), "D5000818000E000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:39:50.705Z"), "FF000079"),
    (("2023-01-16T02:40:16.713Z"), "FF000079"),
    (("2023-01-16T02:40:42.810Z"), "FF000079"),
    (("2023-01-16T02:41:08.957Z"), "D5000818000F000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:41:08.998Z"), "FF000079"),
    (("2023-01-16T02:41:35.022Z"), "FF000079"),
    (("2023-01-16T02:42:01.124Z"), "FF000079"),
    (("2023-01-16T02:42:27.276Z"), "D50008180010000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:42:27.318Z"), "FF00007A"),
    (("2023-01-16T02:42:53.340Z"), "FF00007A"),
    (("2023-01-16T02:42:53.423Z"), "D50008180011000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:43:19.432Z"), "FF00007A"),
    (("2023-01-16T02:43:45.563Z"), "D50008180012000000380000000000660022408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:43:45.605Z"), "FF00007A"),
    (("2023-01-16T02:44:11.609Z"), "FF00007A"),
    (("2023-01-16T02:44:37.693Z"), "FF00007A"),
    (("2023-01-16T02:45:03.825Z"), "D50008180013000000380000000000660022400C4100C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:45:03.867Z"), "FF00007A"),
    (("2023-01-16T02:45:29.875Z"), "FF00007A"),
    (("2023-01-16T02:45:55.963Z"), "FF00007A"),
    (("2023-01-16T02:46:22.095Z"), "D50008180014000000380000000000670042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:46:22.136Z"), "FF00007A"),
    (("2023-01-16T02:46:48.149Z"), "FF00007A"),
    (("2023-01-16T02:47:14.272Z"), "FF00007A"),
    (("2023-01-16T02:47:40.424Z"), "D50008180015000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:47:40.466Z"), "FF00007A"),
    (("2023-01-16T02:48:06.492Z"), "FF00007A"),
    (("2023-01-16T02:48:32.589Z"), "FF00007A"),
    (("2023-01-16T02:48:58.715Z"), "D50008180016000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:48:58.757Z"), "FF00007B"),
    (("2023-01-16T02:49:24.771Z"), "FF00007B"),
    (("2023-01-16T02:49:50.875Z"), "FF00007B"),
    (("2023-01-16T02:50:17.024Z"), "D50008180017000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:50:17.066Z"), "FF00007B"),
    (("2023-01-16T02:50:30.079Z"), "D50008180018000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:50:43.081Z"), "FF00007B"),
    (("2023-01-16T02:51:09.180Z"), "FF00007B"),
    (("2023-01-16T02:51:35.326Z"), "D50008180019000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:51:35.367Z"), "FF00007B"),
    (("2023-01-16T02:52:01.386Z"), "FF00007B"),
    (("2023-01-16T02:52:27.468Z"), "FF00007B"),
    (("2023-01-16T02:52:53.580Z"), "D5000818001A000000380000000000660042408C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:52:53.621Z"), "FF00007B"),
    (("2023-01-16T02:53:19.589Z"), "FF00007B"),
    (("2023-01-16T02:53:45.647Z"), "FF00007B"),
    (("2023-01-16T02:54:11.771Z"), "D5000818001B000000380000000000660042008C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:54:11.813Z"), "FF00007B"),
    (("2023-01-16T02:54:37.812Z"), "FF00007C"),
    (("2023-01-16T02:55:03.885Z"), "FF00007C"),
    (("2023-01-16T02:55:29.988Z"), "D5000818001C000000380000000000670042008C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:55:30.030Z"), "FF00007C"),
    (("2023-01-16T02:55:56.013Z"), "FF00007C"),
    (("2023-01-16T02:56:22.086Z"), "FF00007C"),
    (("2023-01-16T02:56:48.209Z"), "D5000818001D000000380000000000670042000C4100C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:56:48.250Z"), "FF00007C"),
    (("2023-01-16T02:57:14.251Z"), "FF00007C"),
    (("2023-01-16T02:57:40.309Z"), "FF00007C"),
    (("2023-01-16T02:58:06.405Z"), "D5000818001E000000380000000000670042008C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:58:06.447Z"), "FF00007C"),
    (("2023-01-16T02:58:06.529Z"), "D5000818001F000000380000000000670042008C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:58:32.412Z"), "FF00007C"),
    (("2023-01-16T02:58:58.482Z"), "FF00007C"),
    (("2023-01-16T02:59:24.591Z"), "D50008180020000000380000000000670042008C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T02:59:24.633Z"), "FF00007C"),
    (("2023-01-16T02:59:50.617Z"), "FF00007C"),
    (("2023-01-16T03:00:16.676Z"), "FF00007C"),
    (("2023-01-16T03:00:42.798Z"), "D50008180021000000380000000000670042008C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:00:42.839Z"), "FF00007D"),
    (("2023-01-16T03:01:08.839Z"), "FF00007D"),
    (("2023-01-16T03:01:34.912Z"), "FF00007D"),
    (("2023-01-16T03:02:01.027Z"), "D50008180022000000380000000000670042C08B4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:02:01.069Z"), "FF00007D"),
    (("2023-01-16T03:02:27.065Z"), "FF00007D"),
    (("2023-01-16T03:02:53.143Z"), "FF00007D"),
    (("2023-01-16T03:03:19.251Z"), "D50008180023000000380000000000670042C08B4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:03:19.293Z"), "FF00007D"),
    (("2023-01-16T03:03:45.281Z"), "FF00007D"),
    (("2023-01-16T03:04:11.360Z"), "FF00007D"),
    (("2023-01-16T03:04:37.485Z"), "D50008180024000000380000000000670042C08B4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:04:37.527Z"), "FF00007D"),
    (("2023-01-16T03:05:03.524Z"), "FF00007D"),
    (("2023-01-16T03:05:29.597Z"), "FF00007D"),
    (("2023-01-16T03:05:42.682Z"), "D50008180025000000380000000000660022C08B4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:05:55.726Z"), "D50008180026000000380000000000660042008E4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:05:55.767Z"), "FF00007D"),
    (("2023-01-16T03:06:21.774Z"), "FF00007D"),
    (("2023-01-16T03:06:47.847Z"), "FF00007E"),
    (("2023-01-16T03:07:13.962Z"), "D50008180027000000380000000000670042C08B4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:07:14.004Z"), "FF00007E"),
    (("2023-01-16T03:07:39.975Z"), "FF00007E"),
    (("2023-01-16T03:08:06.050Z"), "FF00007E"),
    (("2023-01-16T03:08:32.165Z"), "D50008180028000000380000000000670042808C4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:08:32.206Z"), "FF00007E"),
    (("2023-01-16T03:08:58.189Z"), "FF00007E"),
    (("2023-01-16T03:09:24.285Z"), "FF00007E"),
    (("2023-01-16T03:09:50.429Z"), "D50008180029000000380000000000670041008E4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:09:50.471Z"), "FF00007E"),
    (("2023-01-16T03:10:16.488Z"), "FF00007E"),
    (("2023-01-16T03:10:42.595Z"), "FF00007E"),
    (("2023-01-16T03:11:08.749Z"), "D5000818002A000000380000000000660041008E4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:11:08.790Z"), "FF00007E"),
    (("2023-01-16T03:11:34.793Z"), "FF00007E"),
    (("2023-01-16T03:12:00.881Z"), "FF00007F"),
    (("2023-01-16T03:12:27.023Z"), "D5000818002B000000380000000000660022C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:12:27.065Z"), "FF00007F"),
    (("2023-01-16T03:12:53.078Z"), "FF00007F"),
    (("2023-01-16T03:13:19.165Z"), "FF00007F"),
    (("2023-01-16T03:13:19.248Z"), "D5000818002C000000380000000000660042008E4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:13:45.296Z"), "D5000818002D000000380000000000660021008E4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:13:45.338Z"), "FF00007F"),
    (("2023-01-16T03:14:11.343Z"), "FF00007F"),
    (("2023-01-16T03:14:37.443Z"), "FF00007F"),
    (("2023-01-16T03:15:03.591Z"), "D5000818002E000000380000000000660041008E4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:15:03.633Z"), "FF00007F"),
    (("2023-01-16T03:15:29.642Z"), "FF00007F"),
    (("2023-01-16T03:15:55.753Z"), "FF00007F"),
    (("2023-01-16T03:16:21.916Z"), "D5000818002F000000380000000000660041008E4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:16:21.958Z"), "FF00007F"),
    (("2023-01-16T03:16:47.995Z"), "FF00007F"),
    (("2023-01-16T03:17:14.109Z"), "FF00007F"),
    (("2023-01-16T03:17:40.251Z"), "D50008180030000000380000000000670022C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:17:40.294Z"), "FF000080"),
    (("2023-01-16T03:18:06.297Z"), "FF000080"),
    (("2023-01-16T03:18:32.382Z"), "FF000080"),
    (("2023-01-16T03:18:58.520Z"), "D50008180031000000380000000000660042C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:18:58.563Z"), "FF000080"),
    (("2023-01-16T03:19:24.574Z"), "FF000080"),
    (("2023-01-16T03:19:50.667Z"), "FF000080"),
    (("2023-01-16T03:20:16.792Z"), "D50008180032000000380000000000670042C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:20:16.834Z"), "FF000080"),
    (("2023-01-16T03:20:42.828Z"), "FF000080"),
    (("2023-01-16T03:20:55.915Z"), "D50008180033000000380000000000660042C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:21:08.907Z"), "FF000080"),
    (("2023-01-16T03:21:35.026Z"), "D50008180034000000380000000000660042C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:21:35.068Z"), "FF000080"),
    (("2023-01-16T03:22:01.048Z"), "FF000080"),
    (("2023-01-16T03:22:27.120Z"), "FF000080"),
    (("2023-01-16T03:22:53.219Z"), "D50008180035000000380000000000660042C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:22:53.261Z"), "FF000081"),
    (("2023-01-16T03:23:19.234Z"), "FF000081"),
    (("2023-01-16T03:23:45.293Z"), "FF000081"),
    (("2023-01-16T03:24:11.395Z"), "D50008180036000000380000000000660022C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:24:11.438Z"), "FF000081"),
    (("2023-01-16T03:24:37.411Z"), "FF000081"),
    (("2023-01-16T03:25:03.468Z"), "FF000081"),
    (("2023-01-16T03:25:29.559Z"), "D50008180037000000380000000000660042C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:25:29.602Z"), "FF000081"),
    (("2023-01-16T03:25:55.580Z"), "FF000081"),
    (("2023-01-16T03:26:21.657Z"), "FF000081"),
    (("2023-01-16T03:26:47.772Z"), "D50008180038000000380000000000670022C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:26:47.814Z"), "FF000081"),
    (("2023-01-16T03:27:13.806Z"), "FF000081"),
    (("2023-01-16T03:27:39.880Z"), "FF000081"),
    (("2023-01-16T03:28:05.988Z"), "D50008180039000000380000000000670042C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:28:06.030Z"), "FF000082"),
    (("2023-01-16T03:28:32.013Z"), "FF000082"),
    (("2023-01-16T03:28:32.097Z"), "D5000818003A000000380000000000670042C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:28:58.060Z"), "FF000082"),
    (("2023-01-16T03:29:24.158Z"), "D5000818003B000000380000000000670042C08D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:29:24.201Z"), "FF000082"),
    (("2023-01-16T03:29:50.154Z"), "FF000082"),
    (("2023-01-16T03:30:16.215Z"), "FF000082"),
    (("2023-01-16T03:30:42.324Z"), "D5000818003C000000380000000000660042808D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:30:42.361Z"), "FF000082"),
    (("2023-01-16T03:31:08.347Z"), "FF000082"),
    (("2023-01-16T03:31:34.402Z"), "FF000082"),
    (("2023-01-16T03:32:00.490Z"), "D5000818003D000000380000000000660022808D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:32:00.532Z"), "FF000082"),
    (("2023-01-16T03:32:26.508Z"), "FF000082"),
    (("2023-01-16T03:32:52.553Z"), "FF000082"),
    (("2023-01-16T03:33:18.632Z"), "D5000818003E000000380000000000660042808D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:33:18.673Z"), "FF000083"),
    (("2023-01-16T03:33:44.624Z"), "FF000083"),
    (("2023-01-16T03:34:10.663Z"), "FF000083"),
    (("2023-01-16T03:34:36.763Z"), "D5000818003F000000380000000000660042808D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:34:36.805Z"), "FF000083"),
    (("2023-01-16T03:35:02.798Z"), "FF000083"),
    (("2023-01-16T03:35:28.862Z"), "FF000083"),
    (("2023-01-16T03:35:54.962Z"), "D50008180040000000380000000000660042808D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:35:55.003Z"), "FF000083"),
    (("2023-01-16T03:36:07.995Z"), "D50008180041000000380000000000660022408D4000C4F4010F27FD0CDD0AF3081027342100000000000000000000"),
    (("2023-01-16T03:36:20.980Z"), "FF000083"),
    (("2023-01-16T03:36:47.051Z"), "FF000083")
]

alarms: List[str] = []
tempTimes: list[datetime] = []
tempsDegC: List[float] = []
for t, ps in packets_raw:
    packet = parse_packet(bytes.fromhex(ps))
    msg = (
        f"\n@{t}\n"
        f"{packet!s}"
    )
    alarms.extend(check_packet_for_alarms(packet))
    # Extract telem:
    telem = [
        cast(TelemetryPayload, t) for t in packet.payloads[TelemetryPayload]]
    data = {f'{t.channel.name}': t for t in telem}

    tempRTKey = 'BattAdcTempKelvin'
    if tempRTKey in data:
        tempTimes.append(dateutil.parser.parse(t))
        tempsDegC.append(data[tempRTKey].data - 273.15)

    heaterOnKey = 'Heater_IsHeating'
    if heaterOnKey in data and data[heaterOnKey].data > 0.1:
        print(colored(msg, 'white', 'on_red'))
    else:
        print(msg)

for alarm in alarms:
    print(colored(alarm, 'red'))


df = pd.DataFrame({
    'time': tempTimes,
    'tempDegC': tempsDegC
})
df.set_index('time', inplace=True)

fsize = (14, 8)
fig, axs = plt.subplots(
    nrows=1,
    figsize=fsize,
    dpi=600,
    constrained_layout=True
)
fig.suptitle("Iris Heater Check")

df.plot(
    figsize=fsize,
    grid=True,
    xlabel='generated_time',
    ylabel='Temperature [°C]',
    legend=False,
    marker='o',
    markersize=2,
    linestyle='none'  # just scatter plot these so we know where data is
)
print(f"Saving Results to {PLOT_OUT_FILE} . . .")
plt.savefig(PLOT_OUT_FILE, dpi=600)
plt.close()
