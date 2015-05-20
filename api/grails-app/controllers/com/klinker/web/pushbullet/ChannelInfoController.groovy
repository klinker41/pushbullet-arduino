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

import grails.converters.JSON
import groovyx.net.http.ContentType
import groovyx.net.http.RESTClient

/**
 * Forwards information on a channel to Pushbullet through https://api.pushbullet.com/v2/channel-info
 */
class ChannelInfoController {

    /**
     * Find information about a channel and return it as a JSON. More information can be
     * found at https://docs.pushbullet.com/#subscriptions
     *
     * @return JSON containing channel information
     */
    def index() {
        def client = new RESTClient(PushbulletApi.BASE_URL)
        def resp

        if (request.get) {
            resp = client.get(
                    path: 'v2/channel-info?tag=jblow',
                    requestContentType: ContentType.JSON
            )
        }

        assert resp.status == 200
        render resp.getData() as JSON
        return resp.getData()
    }

}