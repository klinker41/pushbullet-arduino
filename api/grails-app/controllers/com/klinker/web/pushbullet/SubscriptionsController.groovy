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
 * Forwards information on subscriptions to Pushbullet through https://api.pushbullet.com/v2/subscriptions
 */
class SubscriptionsController {

    static allowedMethods = [add:'DELETE']

    /**
     * Find a JSON of the current user object's subscriptions based off of authorization header access token. More
     * information can be found at https://docs.pushbullet.com/#subscriptions.
     *
     * @return JSON containing current user's subscriptions
     */
    def index() {
        def client = new RESTClient(PushbulletApi.BASE_URL)
        def resp

        if (request.get) {
            // GET request, returns all subscriptions
            resp = client.get(
                    path: 'v2/subscriptions',
                    headers: ['Authorization': request.getHeader('Authorization')],
                    requestContentType: ContentType.JSON
            )
        } else if (request.post) {
            // POST request, subscribes to new subscription
            // TODO handle a post request (this should actually subscribe to new subscription)
        } else {
            def id = params.id
            resp = client.delete(
                    path: 'v2/subscriptions/' + id,
                    headers: ['Authorization': request.getHeader('Authorization')],
                    requestContentType: ContentType.JSON,
            )
        }

        assert resp.status == 200
        render resp.getData() as JSON
        return resp.getData()
    }

}
