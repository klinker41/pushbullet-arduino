/*
 * Copyright (C) 2015 Jacob Klinker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package com.klinker.web.pushbullet

/**
 * Contains all API endpoints that app will be able to hit
 */
class PushbulletApi {

    public static final String BASE_URL = 'https://api.pushbullet.com/'
    private static final String VERSION = 'v2'

    public static final String PUSHES_URL = VERSION + '/pushes'
    public static final String DEVICES_URL = VERSION + '/devices'
    public static final String CONTACT_URL = VERSION + '/contacts'
    public static final String SUBSCRIPTIONS_URL = VERSION + '/subscriptions'
    public static final String CHANNEL_INFO_URL = VERSION + '/channel-info'
    public static final String ME_URL = VERSION + '/users/me'

}
